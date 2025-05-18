#include "water.h"

// PROBLEMS
// EDGES HAVE GAPS
// CLUMPS FORM
// ADD NEAR DENSITY AND NEAR PRESSURE FORCES
// https://sph-tutorial.physics-simulation.org/pdf/SPH_Tutorial.pdf

int main() {
    const int screen_width = 800;
    const int screen_height = 600;

    // create the window using GLFW and glad
    GLFWwindow* window = create_window(screen_width, screen_height, "Fluid Simulation");
    glEnable(GL_DEBUG_OUTPUT);

    // vertex and fragment shader for 2 different shader programs
    string vertexShaderStr = loadShaderSource("vertexShader.glsl");
    string fragmentShaderStr = loadShaderSource("fragmentShader.glsl");
    string rayMarchingVertexShaderStr = loadShaderSource("rayMarchingVertexShader.glsl");
    string rayMarchingFragmentShaderStr = loadShaderSource("rayMarchingFragmentShader.glsl");
    string quadVertexShaderStr = loadShaderSource("quadVertexShader.glsl");
    string quadFragmentShaderStr = loadShaderSource("quadFragmentShader.glsl");

    // compute shaders
    string computeShaderStr = loadShaderSource("computeShader.glsl");
    string computePredictedShaderStr = loadShaderSource("computePredictedShader.glsl");
    string computeDensityShaderStr = loadShaderSource("computeDensityShader.glsl");
    string computeNearbySortShaderStr = loadShaderSource("computeNearbySortShader.glsl");
    string computeNearbyIdxShaderStr = loadShaderSource("computeNearbyIdxShader.glsl");
    string fieldDataShaderStr = loadShaderSource("fieldDataShader.glsl");

    // create the two shaders
    vector<GLuint> shaders;
    shaders.push_back(create_shader(vertexShaderStr.c_str(), GL_VERTEX_SHADER));
    shaders.push_back(create_shader(fragmentShaderStr.c_str(), GL_FRAGMENT_SHADER));

    // create raymarching shaders
    vector<GLuint> rayMarchingShaders;
    rayMarchingShaders.push_back(create_shader(rayMarchingVertexShaderStr.c_str(), GL_VERTEX_SHADER));
    rayMarchingShaders.push_back(create_shader(rayMarchingFragmentShaderStr.c_str(), GL_FRAGMENT_SHADER));

    vector<GLuint> quadShaders;
    quadShaders.push_back(create_shader(quadVertexShaderStr.c_str(), GL_VERTEX_SHADER));
    quadShaders.push_back(create_shader(quadFragmentShaderStr.c_str(), GL_FRAGMENT_SHADER));

    // create all of the needed shader programs
    GLuint shaderProgram = create_shader_program(shaders);
    GLuint rayMarchingShaderProgram = create_shader_program(rayMarchingShaders);
    GLuint quadShaderProgram = create_shader_program(quadShaders);
    GLuint computePredictedShaderProgram = create_shader_program(create_shader(computePredictedShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint computeDensityShaderProgram = create_shader_program(create_shader(computeDensityShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint computeShaderProgram = create_shader_program(create_shader(computeShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint computeNearbySortShaderProgram = create_shader_program(create_shader(computeNearbySortShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint computeNearbyIdxShaderProgram = create_shader_program(create_shader(computeNearbyIdxShaderStr.c_str(), GL_COMPUTE_SHADER));
    GLuint fieldDataShaderProgram = create_shader_program(create_shader(fieldDataShaderStr.c_str(), GL_COMPUTE_SHADER));
    
    std::srand(std::time(0));

    vector<vec4> particles;

    // particle system settings
    vec3 min_bound(0.);
    vec3 max_bound(200.);
    vec3 bound_size = max_bound - min_bound;
    vec4 v0Dir = vec4(2.);
    const float sphere_size = .5;
    const float gravity = 9.8;
    const float particle_mass = 1;
    const float particle_damping = .05;
    const float density_radius = 1.9;
    const float target_density = 2.75;
    const float pressure_multiplier = 275;
    const float viscosity_multiplier = .15;

    // TODO particle_count must be power of 2 for the parallel sort -- MUST FIX
    int N = 5;
    int r = pow(2, N);
    int c = pow(2, N);
    int p = pow(2, N);
    //float size = length(max_bound - min_bound) / 2. - 10.;
    float size = 100.;

    create_particle_cube(particles, min_bound, size, r, c, p);
    create_particle_cube(particles, min_bound + vec3(size, size, 0), size, r, c, p);
    const int particles_count = particles.size();

    // ray marching field data settings
    const float field_size = 0.2;
    const int field_rows = ceil(bound_size.x / field_size) + 1;
    const int field_cols = ceil(bound_size.y / field_size) + 1;
    const int field_planes = ceil(bound_size.z / field_size) + 1;
    const int field_data_size = field_rows * field_cols * field_planes;

    // view and proj settings
    vec3 cameraPos = vec3(15., -40., 35.);    
    vec3 cameraTarget = vec3(38., 10., 10.);
    vec3 upVector = vec3(0., 0., 1.);
    const float fovy = glm::radians(65.);
    const float aspectRatio = (float)screen_width / (float)screen_height;
    const float nearPlane = 0.1;
    const float farPlane  = 500.;

    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);
    glm::mat4 projection = glm::perspective(fovy, aspectRatio, nearPlane, farPlane);
    glm::mat4 viewProjection = projection * view;

    const int dispatch_size = particles_count / 32;

    vector<vec3> verts;
    vector<vec3> normals;
    vector<unsigned int> faceList;

    // create basic sphere to be instanced
    create_sphere(verts, normals, faceList, 10, 10, sphere_size);

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

    float quadVerts[] = {
        -1., -1.,
         1., -1.,
        -1.,  1.,
         1.,  1.
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // particle positions
    GLuint particleVBO;
    glGenBuffers(1, &particleVBO);
    
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // create the particles for the compute shader program
    // eventually predicted_particles, nearby, and densities buffers get removed and becomes shared memory instead
    GLuint particles_buffer, velocities_buffer, predicted_particles_buffer, field_data_buffer, nearby_buffer, nearby_idx_buffer, rayMarchingVBO, rayMarchingVAO;
    glGenBuffers(1, &particles_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particles_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * particles_count, particles.data(), GL_STATIC_DRAW);

    // draw spheres at the particle positions
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, particles_buffer);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    vector<vec4> v0(particles_count, v0Dir);
    glGenBuffers(1, &velocities_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, velocities_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * v0.size(), v0.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &predicted_particles_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, predicted_particles_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * particles_count, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &field_data_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, field_data_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * field_data_size, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &nearby_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, nearby_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 2 * particles_count, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &nearby_idx_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, nearby_idx_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * particles_count, nullptr, GL_STATIC_DRAW);
    
    const float full_screen_vertices[] = {-1.0f, -1.0f, 0.0f,  
                        1.0f, -1.0f, 0.0f, 
                        -1.0f, 1.0f, 0.0f,
                        1.0f, 1.0f, 0.0f
                        };
                        
    glGenVertexArrays(1, &rayMarchingVAO);
    glGenBuffers(1, &rayMarchingVBO);

    glBindVertexArray(rayMarchingVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rayMarchingVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(full_screen_vertices), full_screen_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup constant uniforms for each of the shader program
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));

    glUseProgram(quadShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(quadShaderProgram, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniformMatrix4fv(glGetUniformLocation(quadShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(glGetUniformLocation(quadShaderProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform1f(glGetUniformLocation(quadShaderProgram, "size"), 1);

    glUseProgram(computePredictedShaderProgram);
    glUniform1i(glGetUniformLocation(computePredictedShaderProgram, "particles_count"), particles_count);
    glUniform1f(glGetUniformLocation(computePredictedShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(computePredictedShaderProgram, "gravity"), gravity);

    glUseProgram(computeNearbyIdxShaderProgram);
    glUniform1i(glGetUniformLocation(computeNearbyIdxShaderProgram, "particles_count"), particles_count);

    glUseProgram(computeDensityShaderProgram);
    glUniform1i(glGetUniformLocation(computeDensityShaderProgram, "particles_count"), particles_count);
    glUniform1f(glGetUniformLocation(computeDensityShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(computeDensityShaderProgram, "particle_mass"), particle_mass);

    glUseProgram(computeShaderProgram);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "particles_count"), particles_count);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "target_density"), target_density);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_mass"), particle_mass);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "pressure_multiplier"), pressure_multiplier);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "viscosity_multiplier"), viscosity_multiplier);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "particle_damping"), particle_damping);
    glUniform3fv(glGetUniformLocation(computeShaderProgram, "min_bound"), 1, glm::value_ptr(min_bound));
    glUniform3fv(glGetUniformLocation(computeShaderProgram, "max_bound"), 1, glm::value_ptr(max_bound));

    glUseProgram(fieldDataShaderProgram);
    glUniform1i(glGetUniformLocation(fieldDataShaderProgram, "particles_count"), particles_count);
    glUniform1i(glGetUniformLocation(fieldDataShaderProgram, "field_rows"), field_rows);
    glUniform1i(glGetUniformLocation(fieldDataShaderProgram, "field_cols"), field_cols);
    glUniform1i(glGetUniformLocation(fieldDataShaderProgram, "field_planes"), field_planes);
    glUniform1f(glGetUniformLocation(fieldDataShaderProgram, "field_size"), field_size);
    glUniform1f(glGetUniformLocation(fieldDataShaderProgram, "density_radius"), density_radius);
    glUniform1f(glGetUniformLocation(fieldDataShaderProgram, "particle_mass"), particle_mass);
    glUniform3fv(glGetUniformLocation(fieldDataShaderProgram, "min_bound"), 1, glm::value_ptr(min_bound));

    glUseProgram(rayMarchingShaderProgram);
    glUniform1i(glGetUniformLocation(rayMarchingShaderProgram, "field_rows"), field_rows);
    glUniform1i(glGetUniformLocation(rayMarchingShaderProgram, "field_cols"), field_cols);
    glUniform1i(glGetUniformLocation(rayMarchingShaderProgram, "field_planes"), field_planes);
    glUniform1f(glGetUniformLocation(rayMarchingShaderProgram, "field_size"), field_size);
    glUniform3fv(glGetUniformLocation(rayMarchingShaderProgram, "min_bound"), 1, glm::value_ptr(min_bound));
    glUniform3fv(glGetUniformLocation(rayMarchingShaderProgram, "max_bound"), 1, glm::value_ptr(max_bound));
    glUniform2f(glGetUniformLocation(rayMarchingShaderProgram, "u_resolution"), (float)screen_width, (float)screen_height);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int frames = 0;
    double last_frame_time = 0;
    double last_fps_print_time = 0.;

    while (!glfwWindowShouldClose(window)) {
        double currTime = glfwGetTime();
        double dt = currTime - last_frame_time;

        // this prevents massive spikes from moving the screen 
        // since it pauses also from low fps
        dt = std::min(dt, .05);

        last_frame_time = currTime;

        processInput(window);
        glClearColor(0., 0., 0., 1.);  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create the spatial lookup table for this frame for all the future shaders to use
        // and create predicted particles for better particle position estimation
        glUseProgram(computePredictedShaderProgram);
        glUniform1f(glGetUniformLocation(computePredictedShaderProgram, "dt"), dt);
        glDispatchCompute(dispatch_size, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // sort the spatial look up table called nearby
        // only works in particles_count is a power of 2
        // TODO change so that array length can be any length
        glUseProgram(computeNearbySortShaderProgram);
        GLuint kLoc = glGetUniformLocation(computeNearbySortShaderProgram, "k");
        GLuint jLoc = glGetUniformLocation(computeNearbySortShaderProgram, "j");
        for (int k = 2; k <= particles_count; k *= 2) {
            for (int j = k / 2; j > 0; j /= 2) {
                glUniform1i(kLoc, k);
                glUniform1i(jLoc, j);
                
                glDispatchCompute(dispatch_size, 1, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            }
        }

        // used to map to correct grid coord the particle belongs to in nearby
        glUseProgram(computeNearbyIdxShaderProgram);
        glDispatchCompute(dispatch_size, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // run density calculations compute shader
        glUseProgram(computeDensityShaderProgram);
        glDispatchCompute(dispatch_size, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // run forces calculations compute shader
        glUseProgram(computeShaderProgram);
        glUniform1f(glGetUniformLocation(computeShaderProgram, "dt"), dt);
        glDispatchCompute(dispatch_size, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        // draw spheres
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, faceList.size(), GL_UNSIGNED_INT, 0, particles_count);

        // draw quads
        // glUseProgram(quadShaderProgram);
        // glBindVertexArray(quadVAO);
        // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles_count);

        // create the density field
        // glUseProgram(fieldDataShaderProgram);
        // glDispatchCompute(field_rows / 4, field_cols / 4, field_planes / 4);
        // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // // draw full screen for rayMarching
        // glUseProgram(rayMarchingShaderProgram);

        // double xpos;
        // double ypos;
        // glfwGetCursorPos(window, &xpos, &ypos);
        // glUniform2f(glGetUniformLocation(rayMarchingShaderProgram, "u_mouse"), (float)xpos, (float)ypos);

        // glBindVertexArray(rayMarchingVAO);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

        ++frames;

        // print fps
        if(currTime - last_fps_print_time > .5) {
            cout << (int)(frames / currTime) << endl;
            last_fps_print_time = currTime;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}