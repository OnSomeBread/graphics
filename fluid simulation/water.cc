#include "marching_cubes.cc"
//#include "water.h"

using std::max;
using std::pow;
using std::round;
using std::sqrt;
using std::unordered_map;
using std::vector;

// PROBLEMS
// STARTING POS IS AGGRESSIVE
// EDGES HAVE GAPS
// CLUMPS FORM
// IMPLEMENT MARCHING CUBES
// ADD NEAR DENSITY AND NEAR PRESSURE FORCES
// https://sph-tutorial.physics-simulation.org/pdf/SPH_Tutorial.pdf

// massive optimization to allow for more particles
// split the bounding box into grid squares of size density_radius
// then when calculating densities or pressure forces only consider the
// 3x3 grid of cells since the rest of the particles will add 0 anyway
// bits 0-7 planes, bits 7-15 cols, and bits 16-23 is rows
unordered_map<int, vector<vec3>> grid;
int grid_size = 1000;

// the spatial
int hash_function(int x, int y, int z) {
    return ((x * 73856093) xor (y * 19349663) xor (z * 83492791)) % grid_size;
}

float magnitude(vec3 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

// cubic smoothing function
double smoothing(double radius, double diff) {
    if (diff < radius) {
        double volume = (64 * M_PI * pow(radius, 9)) / 315.0;
        return pow(radius * radius - diff * diff, 3) / volume;
    }
    return 0;
}

float steeperSmoothing(float dst, float radius) {
    if (dst < radius) {
        double volume = (M_PI * pow(radius, 6)) / 15.0;
        return pow(radius - dst, 3) * volume;
    }
    return 0;
}

// derivative of the smoothing function
double dsmoothing(double radius, double diff) {
    if (diff < radius) {
        double volume = (64 * M_PI * pow(radius, 9)) / 315.0;
        return -6 * pow(radius * radius - diff * diff, 2) * diff / volume;
    }
    return 0;
}

float DsteeperSmoothing(float dst, float radius) {
    if (dst <= radius) {
        double volume = (pow(radius, 6) * M_PI) / 35.0;
        return -pow(radius - dst, 2) / volume;
    }
    return 0;
}

vector<vec3> get_nearest_particles(vec3 p, float density_radius) {
    vector<vec3> nearby;
    vec3 idx = p / density_radius;

    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; ++k) {
                int grid_idx = hash_function((int)idx.x + i, (int)idx.y + j,
                                             (int)idx.z + k);
                nearby.insert(nearby.end(), grid[grid_idx].begin(),
                              grid[grid_idx].end());
            }
        }
    }
    return nearby;
}

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
float calc_density(vector<vec3>& particles, vec3 particle, float radius,
                    float mass) {
    double density = 0;
    for (int i = 0; i < (int)particles.size(); ++i) {
        float d = magnitude(particles[i] - particle);
        density += mass * smoothing(radius, d);
    }
    return density;
}

// used to create a t value for interpolate ie a value between 0-1
float scale_t_val(float value, float data_min, float data_max) {
    return (value - data_min) / (data_max - data_min);
}

int main() {
    int screen_width = 800;
    int screen_height = 600;

    // create the window using GLFW and glad
    GLFWwindow* window = create_window(screen_width, screen_height, "Fluid Simulation");
    glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(myCallbackFunc, nullptr);

    std::string vertexShaderStr = loadShaderSource("vertexShader.glsl");
    std::string fragmentShaderStr = loadShaderSource("fragmentShader.glsl");
    std::string computeShaderStr = loadShaderSource("computeShader.glsl");

    // create the two shaders
    GLuint vertexShader = create_shader(vertexShaderStr.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = create_shader(fragmentShaderStr.c_str(), GL_FRAGMENT_SHADER);

    // link the two shaders into a shader program
    int success;
    char infoLog[512];
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "shader program linking failed:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint computeShader = create_shader(computeShaderStr.c_str(), GL_COMPUTE_SHADER);
    GLuint computeShaderProgram = glCreateProgram();
    glAttachShader(computeShaderProgram, computeShader);
    glLinkProgram(computeShaderProgram);

    glGetProgramiv(computeShaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(computeShaderProgram, 512, nullptr, infoLog);
        std::cerr << "compute shader program linking failed:\n" << infoLog << "\n";
    }

    glDeleteShader(computeShader);

    std::srand(std::time(0));

    vector<vec4> particles;
    vector<vec4> velocities;
    vector<vec4> predicted_particles;
    //vector<float> densities;

    vec3 min_bound(0.);
    vec3 max_bound(40.);

    create_particle_system(particles, predicted_particles, velocities, min_bound, max_bound, 12, 12, 12);

    float sphere_size = .4;
    // vec3 slow_particle_color = {.1, .25, 1};
    // vec3 fast_particle_color = {.25, .55, .95};
    // float fast_v = 4;  // highest value for color

    float g = .8;
    float particle_mass = 1;

    float particle_damping = .05;
    float density_radius = 1.9;
    float target_density = 2.75;

    // for the marching cubes algo
    // float surfacelvl = .2;
    // float field_size = 2;
    // int field_rows = bound_size.x / field_size + 1.0f;
    // int field_cols = bound_size.y / field_size + 1.0f;
    // int field_planes = bound_size.z / field_size + 1.0f;
    // vector<vector<vector<float>>> data(
    //     field_rows,
    //     vector<vector<float>>(field_cols, vector<float>(field_planes, 0)));

    // how fast do we want particles to be target_density
    float pressure_multiplier = 30;
    float viscosity_multiplier = .4;

    vector<vec3> verts;
    vector<vec3> normals;
    vector<unsigned int> faceList;

    // create basic sphere to be instanced
    create_sphere(verts, normals, faceList, 16, 16, sphere_size, vec3(0));

    // create the buffers for the shader program
    GLuint vao, vboPos, vboNorm, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // sphere positions
    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    // sphere normals
    glGenBuffers(1, &vboNorm);
    glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    // sphere faceList
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceList.size() * sizeof(unsigned int), faceList.data(), GL_STATIC_DRAW);

    // particle positions
    GLuint particleVBO;
    glGenBuffers(1, &particleVBO);
    
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // create the particles for the compute shader program
    // eventually predicted_particles, nearby, and densities buffers get removed and becomes shared memory instead
    GLuint particles_buffer, velocities_buffer, predicted_particles_buffer, nearby_buffer;
    glGenBuffers(1, &particles_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particles_buffer);

    glGenBuffers(1, &velocities_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, velocities_buffer);

    glGenBuffers(1, &predicted_particles_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, predicted_particles_buffer);

    glGenBuffers(1, &nearby_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, nearby_buffer);

    glEnable(GL_DEPTH_TEST);

    glm::vec3 cameraPos = glm::vec3(-4.0f, -15.0f, 16.0f);    
    glm::vec3 cameraTarget = glm::vec3(5.0f, 0.0f, 6.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);

    float fov = glm::radians(40.0f);
    float aspectRatio = (float)screen_width / (float)screen_height;
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;

    glm::mat4 projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

    int frames = 0;
    auto last_frame_time = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto curr = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        curr - last_frame_time)
                        .count() /
                    1000000.0f;

        // this prevents massive spikes from moving the screen 
        // since it pauses also from low fps
        dt = std::min(dt, .1f);

        last_frame_time = curr;

        processInput(window);
        
        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // predicted_particles, densities, nearby_particles should all be shared compute memory
        // particles, velocities will be input, output

        // add gravitational forces to particles
        for (int i = 0; i < (int)particles.size(); ++i) {
            velocities[i].z -= g * dt;
            predicted_particles[i] = particles[i] + velocities[i] * dt;
        }

        // populate the particle grid
        grid.clear();
        for (int i = 0; i < (int)predicted_particles.size(); ++i) {
            vec3 idx = vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z) / density_radius;
            int grid_idx = hash_function((int)idx.x, (int)idx.y, (int)idx.z);
            grid[grid_idx].push_back(vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z));
        }

        vector<vector<vec3>> nearby_particles;
        for (int i = 0; i < (int)particles.size(); ++i) {
            nearby_particles.push_back(get_nearest_particles(vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z), density_radius));
        }

        // create densities table for it to be used in pressure forces
        for (int i = 0; i < (int)particles.size(); ++i) {
            predicted_particles[i].w = calc_density(nearby_particles[i], vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z),
                                        density_radius, particle_mass);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * particles.size(), particles.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particles_buffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocities_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * velocities.size(), velocities.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, velocities_buffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, predicted_particles_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * predicted_particles.size(), predicted_particles.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, predicted_particles_buffer);

        // glBindBuffer(GL_SHADER_STORAGE_BUFFER, nearby_buffer);
        // glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * velocities.size(), velocities.data(), GL_STATIC_DRAW);

        //glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glUseProgram(computeShaderProgram);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        

        glUniform1i(glGetUniformLocation(computeShaderProgram, "particles_count"), particles.size());
        glUniform1f(glGetUniformLocation(computeShaderProgram, "target_density"), target_density);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "density_radius"), density_radius);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_mass"), particle_mass);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "pressure_multiplier"), pressure_multiplier);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "viscosity_multiplier"), viscosity_multiplier);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "dt"), dt);

        glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_damping"), particle_damping);
        glUniform3fv(glGetUniformLocation(computeShaderProgram, "min_bound"), 1, glm::value_ptr(min_bound));
        glUniform3fv(glGetUniformLocation(computeShaderProgram, "max_bound"), 1, glm::value_ptr(max_bound));

        glDispatchCompute((particles.size()-31) / 32, 1, 1);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles_buffer);
        glm::vec4* ptr1 = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocities_buffer);
        glm::vec4* ptr2 = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

        for (int i = 0; i < (int)particles.size(); ++i) {
            particles[i] = ptr1[i];
            velocities[i] = ptr2[i];
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        
        // for (int i = 0; i < field_rows; ++i) {
        //     for (int j = 0; j < field_cols; ++j) {
        //         for (int k = 0; k < field_planes; ++k) {
        //             vec3 field_particle = {
        //                 (float)i * field_size + field_size * .5,
        //                 (float)j * field_size + field_size * .5,
        //                 (float)k * field_size + field_size * .5};

        //             vector<vec3> nearby =
        //                 get_nearest_particles(field_particle,
        //                 density_radius);
        //             data[i][j][k] = calc_density(nearby, field_particle,
        //                                          density_radius,
        //                                          particle_mass);
        //         }
        //     }
        // }

        // marching_cubes(data, surfacelvl);

        // draw spheres at the particle positions
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec4), particles.data(), GL_STATIC_DRAW);

        // make it instanced spheres
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);
        
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, faceList.size(), GL_UNSIGNED_INT, 0, particles.size());

        glfwSwapBuffers(window);
        glfwPollEvents();

        frames++;

        //std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if(frames % 10 == 0) {
            std::cout << (int)(frames / glfwGetTime()) << std::endl;
        }
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboNorm);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &particleVBO);

    glDeleteBuffers(1, &particles_buffer);
    glDeleteBuffers(1, &velocities_buffer);
    glDeleteBuffers(1, &predicted_particles_buffer);
    glDeleteBuffers(1, &nearby_buffer);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(computeShaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}