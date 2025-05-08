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

// calculate the pressure force for the particle
vec3 pressure_force(vector<vec3>& particles, int pointidx,
                  vector<float>& densities, float target_density, float radius,
                  float mass) {
    vec3 pressure(0,0,0);

    for (int i = 0; i < (int)particles.size(); ++i) {
        // cant use the current particle otherwise creates nans
        if (i == pointidx) continue;

        vec3 particle_diff = particles[i] - particles[pointidx];
        float d = magnitude(particle_diff);
        float ds = dsmoothing(radius, d);

        // if moving in same direction pick a random one instead
        vec3 dir = d == 0 ? random_dir() : particle_diff / d;

        // instead of just p0 use the average particle pressure between the
        // current particle and this particle
        float p0 = (target_density - densities[i]);
        float p1 = (target_density - densities[pointidx]);
        float avgp = (p0 + p1) / 2.0;

        pressure += dir * avgp * mass * ds / (float)densities[i];
    }
    return pressure;
}

vec3 viscosity_force(vector<vec3>& particles, int pointidx, vector<vec3>& velocities,
                   float radius) {
    vec3 viscosity(0,0,0);
    for (int i = 0; i < (int)particles.size(); ++i) {
        if (i == pointidx) continue;
        float d = magnitude(particles[pointidx] - particles[i]);
        float s = smoothing(radius, d);
        viscosity += (velocities[i] - velocities[pointidx]) * s;
    }

    return viscosity;
}

// used to create a t value for interpolate ie a value between 0-1
float scale_t_val(float value, float data_min, float data_max) {
    return (value - data_min) / (data_max - data_min);
}

void pr(vec3 p){
    cout << p.x << " " << p.y << " " << p.z << endl;
}

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()){
        cout << "Failed to open shader file" << endl;
        return "";
    }
        
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    int screen_width = 800;
    int screen_height = 600;

    std::string vertexShaderStr = loadShaderSource("vertexShader.glsl");
    std::string fragmentShaderStr = loadShaderSource("fragmentShader.glsl");
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(screen_width, screen_height, "Basic Shader", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, screen_width, screen_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int success;
    char infoLog[512];
    // create the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = vertexShaderStr.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Compilation Failed:\n" << infoLog << "\n";
    }

    // create the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = fragmentShaderStr.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Compilation Failed:\n" << infoLog << "\n";
    }

    // link the two shaders into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //std::time(0)
    std::srand(1);

    vector<vec3> particles;
    vector<vec3> velocities;
    vector<vec3> predicted_particles;
    vector<float> densities;

    int rows = 8;
    int cols = 8;
    int planes = 8;

    // min and max of the boundary box that the particles should be contained in
    vec3 min_bound(0., 0., 0.);
    vec3 max_bound(21., 21., 21.);
    vec3 bound_size(max_bound.x - min_bound.x, max_bound.y - min_bound.y,
                     max_bound.z - min_bound.z);

    vec3 extra(0., 5., 5.);

    vec3 pos = min_bound;
    vec3 spacing((bound_size.x - extra.x) / rows,
                  (bound_size.y - extra.y) / cols,
                  (bound_size.z - extra.z) / planes);

    // make a particle grid with some randomization in how they are placed along
    // the grid
    for (int i = 0; i < rows; ++i) {
        pos.x = i * spacing.x;
        for (int j = 0; j < cols; ++j) {
            pos.y = j * spacing.y;
            for (int k = 0; k < planes; ++k) {
                pos.x += random_float(-spacing.x / 3.0, spacing.x / 3.0);
                pos.y += random_float(-spacing.y / 3.0, spacing.y / 3.0);
                pos.z = k * spacing.z +
                        random_float(-spacing.z / 3.0, spacing.z / 3.0);
                particles.push_back(pos);
                predicted_particles.push_back(pos);
                velocities.push_back(vec3(0, 0, 0));
                densities.push_back(0);
            }
        }
    }

    float sphere_size = .4;
    // vec3 slow_particle_color = {.1, .25, 1};
    // vec3 fast_particle_color = {.25, .55, .95};
    // float fast_v = 4;  // highest value for color

    float g = .8;
    float particle_mass = 1;

    float particle_damping = .05;
    float density_radius = 1.9;
    float target_density = 2;

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

    GLuint vao, vboPos, vboNorm, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    vector<vec3> verts;
    vector<vec3> normals;
    vector<unsigned int> faceList;

    // create basic sphere to be instanced
    create_sphere(verts, normals, faceList, 8, 8, sphere_size, vec3(0));
    
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

        // add gravitational forces to particles
        for (int i = 0; i < (int)particles.size(); ++i) {
            velocities[i].z += -g * dt;
            predicted_particles[i] = particles[i] + velocities[i] * dt;
        }

        // populate the particle grid
        grid.clear();
        for (int i = 0; i < (int)predicted_particles.size(); ++i) {
            vec3 idx = predicted_particles[i] / density_radius;
            int grid_idx = hash_function((int)idx.x, (int)idx.y, (int)idx.z);
            grid[grid_idx].push_back(predicted_particles[i]);
        }

        // create densities table for it to be used in pressure forces
        for (int i = 0; i < (int)particles.size(); ++i) {
            vector<vec3> nearby =
                get_nearest_particles(predicted_particles[i], density_radius);
            densities[i] = calc_density(nearby, predicted_particles[i],
                                        density_radius, particle_mass);
        }

        // add particle pressure forces
        // tells the particle how fast it should conform to target density
        for (int i = 0; i < (int)particles.size(); ++i) {
            vec3 pressure_accel =
                pressure_force(predicted_particles, i, densities,
                               target_density, density_radius, particle_mass) *
                pressure_multiplier / densities[i];

            velocities[i] += pressure_accel * dt;
        }

        // add viscosity force
        // creates friction between nearby particles so that particles within
        // the radius have similar velocities
        for (int i = 0; i < (int)particles.size(); ++i) {
            velocities[i] +=
                viscosity_force(particles, i, velocities, density_radius) *
                viscosity_multiplier;
        }

        for (int i = 0; i < (int)particles.size(); ++i) {
            particles[i] += velocities[i] * dt;
            bounds_check(particles[i], velocities[i], particle_damping,
                         min_bound, max_bound);
        }
        
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
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), particles.data(), GL_DYNAMIC_DRAW);

        // make it instanced spheres
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);
        
        glUseProgram(shaderProgram);

        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

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
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// changes particle pos and velocity if out of bounds
void bounds_check(vec3& point, vec3& v, float damping, vec3 min_pos, vec3 max_pos) {
    if (point.x < min_pos.x) {
        point.x = min_pos.x;
        v.x *= -damping;
    }
    if (point.x > max_pos.x) {
        point.x = max_pos.x;
        v.x *= -damping;
    }

    if (point.y < min_pos.y) {
        point.y = min_pos.y;
        v.y *= -damping;
    }
    if (point.y > max_pos.y) {
        point.y = max_pos.y;
        v.y *= -damping;
    }

    if (point.z < min_pos.z) {
        point.z = min_pos.z;
        v.z *= -damping;
    }
    if (point.z > max_pos.z) {
        point.z = max_pos.z;
        v.z *= -damping;
    }
}

void create_sphere(vector<vec3>& verts, vector<vec3>& normals, vector<unsigned int>& faceList, int xpartitions, int ypartitions, float radius, vec3 offset) {
    for (int i = 0; i < ypartitions + 1; ++i) {
        double u = (2.0 * M_PI * i) / (double)ypartitions;
        for (int j = 0; j < xpartitions + 1; ++j) {
            double v = (M_PI * j) / (xpartitions / 2.0) - (M_PI / 2.0);
            float cu = cos(u);
            float su = sin(u);
            float cv = cos(v);
            float sv = sin(v);
        
            verts.push_back(radius * vec3(cv * cu, cv * su, sv) + offset);

            // haha normal time
            // partial derivative with respect to u
            vec3 du(cv * -su, cv * cu, 0);
            du *= radius;

            // partial derivative with respect to v
            vec3 dv(-sv * cu, -sv * su, cv);
            dv *= radius;

            // du X dv
            float nx = du.y * dv.z - du.z * dv.y;
            float ny = du.z * dv.x - du.x * dv.z;
            float nz = du.x * dv.y - du.y * dv.x;

            // normalize
            normals.push_back(vec3(nx, ny, nz) / std::sqrt(nx * nx + ny * ny + nz * nz));
        }
    }

    // create the facelist
    for (int i = 0; i < ypartitions; ++i) {
        for (int j = 0; j < xpartitions; ++j) {
            int i1 = i * (xpartitions + 1) + j;
            int i2 = (i + 1) * (xpartitions + 1) + j;
    
            faceList.push_back(i1);
            faceList.push_back(i2);
            faceList.push_back(i1 + 1);
    
            faceList.push_back(i1 + 1);
            faceList.push_back(i2);
            faceList.push_back(i2 + 1);
        }
    }
}