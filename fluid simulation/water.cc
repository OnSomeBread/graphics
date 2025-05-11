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

int main() {
    int screen_width = 800;
    int screen_height = 600;

    // create the window using GLFW and glad
    GLFWwindow* window = create_window(screen_width, screen_height, "Fluid Simulation");

    std::string vertexShaderStr = loadShaderSource("vertexShader.glsl");
    std::string fragmentShaderStr = loadShaderSource("fragmentShader.glsl");
    std::string computeShaderStr = loadShaderSource("computeShader.glsl");
    std::string computePredictedShaderStr = loadShaderSource("computePredictedShader.glsl");
    std::string computeDensityShaderStr = loadShaderSource("computeDensityShader.glsl");

    std::string fieldDataShaderStr = loadShaderSource("fieldDataShader.glsl");
    std::string marchingCubesShaderStr = loadShaderSource("marchingCubes.glsl");

    // create the two shaders
    vector<GLuint> shaders;
    shaders.push_back(create_shader(vertexShaderStr.c_str(), GL_VERTEX_SHADER));
    shaders.push_back(create_shader(fragmentShaderStr.c_str(), GL_FRAGMENT_SHADER));

    // create all of the needed shader programs
    GLuint shaderProgram = create_shader_program(shaders);
    GLuint computePredictedShaderProgram = create_shader_program(create_shader(computePredictedShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint computeDensityShaderProgram = create_shader_program(create_shader(computeDensityShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint computeShaderProgram = create_shader_program(create_shader(computeShaderStr.c_str(), GL_COMPUTE_SHADER));

    GLuint fieldDataShaderProgram = create_shader_program(create_shader(fieldDataShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint marchingCubesShaderProgram = create_shader_program(create_shader(marchingCubesShaderStr.c_str(), GL_COMPUTE_SHADER));

    std::srand(std::time(0));

    vector<vec4> particles;
    vector<vec4> velocities;

    vec3 min_bound(0.);
    vec3 max_bound(100.);

    create_particle_system(particles, velocities, min_bound, max_bound, 21, 21, 21);

    float sphere_size = .7;
    float gravity = 1;
    float particle_mass = 1;
    float particle_damping = .05;
    float density_radius = 1.9;
    float target_density = 2.75;
    float pressure_multiplier = 30;
    float viscosity_multiplier = .4;

    // for the marching cubes algo
    vec3 bound_size = max_bound - min_bound;
    float surfacelvl = .2;
    float field_size = 2;
    int field_rows = ceil(bound_size.x / field_size) + 1;
    int field_cols = ceil(bound_size.y / field_size) + 1;
    int field_planes = ceil(bound_size.z / field_size) + 1;
    int field_data_size =  (field_rows - 1) * (field_cols - 1) * (field_planes - 1);
    // vector<vector<vector<float>>> data(
    //     field_rows,
    //     vector<vector<float>>(field_cols, vector<float>(field_planes, 0)));

    // view and proj settings
    vec3 cameraPos = vec3(-4.0f, -15.0f, 25.0f);    
    vec3 cameraTarget = vec3(15.0f, 10.0f, 10.0f);
    vec3 upVector = vec3(0.0f, 0.0f, 1.0f);
    float fov = glm::radians(60.0f);
    float aspectRatio = (float)screen_width / (float)screen_height;
    float nearPlane = 0.1f;
    float farPlane  = 500.0f;

    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);
    glm::mat4 projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

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
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec3), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    // sphere normals
    glGenBuffers(1, &vboNorm);
    glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
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
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * particles.size(), particles.data(), GL_STATIC_DRAW);

    // draw spheres at the particle positions
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, particles_buffer);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glGenBuffers(1, &velocities_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, velocities_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * velocities.size(), velocities.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &predicted_particles_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, predicted_particles_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * particles.size(), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &nearby_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, nearby_buffer);

    GLuint field_data_buffer, triTable_buffer, edges_buffer, edgeTable_buffer, coords_buffer, normals_buffer, faceidx_buffer;
    // marching cubes input
    glGenBuffers(1, &field_data_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, field_data_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * field_data_size, 0, GL_STATIC_DRAW);

    glGenBuffers(1, &triTable_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, triTable_buffer);
    int flatTriTable[256 * 16];
    for(int i = 0; i < 256; ++i){
        for(int j = 0; j < 16; ++j){
            flatTriTable[i * 16 + j] = triTable[i][j];
        }
    }
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256 * 16, flatTriTable, GL_STATIC_DRAW);

    glGenBuffers(1, &edges_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, edges_buffer);
    int flatEdges[12 * 2];
    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 2; ++j){
            flatEdges[i * 2 + j] = edges[i][j];
        }
    }
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 12 * 2, flatEdges, GL_STATIC_DRAW);

    glGenBuffers(1, &edgeTable_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, edgeTable_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256, edgeTable, GL_STATIC_DRAW);

    // marching cubes output 
    // there is at most 5 triangles for any cube config so since
    // gpus need static buffers calculate total number of cubes in the field mult by 15 and that
    // is the most amount of coords possible 
    glGenBuffers(1, &coords_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, coords_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * field_data_size * 15, 0, GL_STATIC_DRAW);

    glGenBuffers(1, &normals_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, normals_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * field_data_size * 15, 0, GL_STATIC_DRAW);

    glGenBuffers(1, &faceidx_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, faceidx_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uint) * field_data_size * 15, 0, GL_STATIC_DRAW);

    // setup constant uniforms for each of the shader program
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUseProgram(computePredictedShaderProgram);
    glUniform1i(glGetUniformLocation(computePredictedShaderProgram, "particles_count"), particles.size());
    glUniform1f(glGetUniformLocation(computePredictedShaderProgram, "gravity"), gravity);

    glUseProgram(computeDensityShaderProgram);
    glUniform1i(glGetUniformLocation(computeDensityShaderProgram, "particles_count"), particles.size());
    glUniform1f(glGetUniformLocation(computeDensityShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(computeDensityShaderProgram, "particle_mass"), particle_mass);

    glUseProgram(computeShaderProgram);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "particles_count"), particles.size());
    glUniform1f(glGetUniformLocation(computeShaderProgram, "target_density"), target_density);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_mass"), particle_mass);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "pressure_multiplier"), pressure_multiplier);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "viscosity_multiplier"), viscosity_multiplier);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_damping"), particle_damping);
    glUniform3fv(glGetUniformLocation(computeShaderProgram, "min_bound"), 1, glm::value_ptr(min_bound));
    glUniform3fv(glGetUniformLocation(computeShaderProgram, "max_bound"), 1, glm::value_ptr(max_bound));

    glUseProgram(fieldDataShaderProgram);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "field_rows"), field_rows);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "field_cols"), field_cols);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "field_planes"), field_planes);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "field_size"), field_size);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_mass"), particle_mass);

    glEnable(GL_DEPTH_TEST);

    int frames = 0;
    auto last_frame_time = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto curr = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        curr - last_frame_time)
                        .count() /
                    1000000.0;

        // this prevents massive spikes from moving the screen 
        // since it pauses also from low fps
        dt = std::min(dt, .1f);

        last_frame_time = curr;

        processInput(window);
        
        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // populate the particle grid
        // grid.clear();
        // for (int i = 0; i < (int)predicted_particles.size(); ++i) {
        //     vec3 idx = vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z) / density_radius;
        //     int grid_idx = hash_function((int)idx.x, (int)idx.y, (int)idx.z);
        //     grid[grid_idx].push_back(vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z));
        // }

        // vector<vector<vec3>> nearby_particles;
        // for (int i = 0; i < (int)particles.size(); ++i) {
        //     nearby_particles.push_back(get_nearest_particles(vec3(predicted_particles[i].x, predicted_particles[i].y, predicted_particles[i].z), density_radius));
        // }

        // run predicted particles compute shader
        glUseProgram(computePredictedShaderProgram);
        glUniform1f(glGetUniformLocation(computePredictedShaderProgram, "dt"), dt);
        glDispatchCompute(particles.size() / 32, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // run density calculations compute shader
        glUseProgram(computeDensityShaderProgram);
        glUniform1f(glGetUniformLocation(computeDensityShaderProgram, "dt"), dt);
        glDispatchCompute(particles.size() / 32, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // run forces calculations compute shader
        glUseProgram(computeShaderProgram);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "dt"), dt);
        glDispatchCompute(particles.size() / 32, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // create the density field for the marching cubes algorithm
        glUseProgram(fieldDataShaderProgram);
        glDispatchCompute(field_rows / 2, field_cols / 2, field_planes / 2);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // glUseProgram(marchingCubesShaderProgram);

        // marching_cubes(data, surfacelvl);
        
        glUseProgram(shaderProgram);

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
    glDeleteProgram(computePredictedShaderProgram);
    glDeleteProgram(computeDensityShaderProgram);
    glDeleteProgram(computeShaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}