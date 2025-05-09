#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using std::vector;
using glm::vec3;
using glm::vec4;
using std::cout;
using std::endl;

vec3 interpolate(vec3 start, vec3 end, float t) { return (end - start) * t + start; }

void bounds_check(vec3& point, vec3& v, float damping, vec3 min_pos, vec3 max_pos);

float random_float(float low, float high) {
    return low + static_cast<float>(rand()) /
                     (static_cast<float>(RAND_MAX / (high - low)));
}

vec3 random_dir() {
    return {(float)(rand() % 3 - 1), (float)(rand() % 3 - 1),
            (float)(rand() % 3 - 1)};
}

vec3 random_dir_float() {
    return {random_float(-1, 1), random_float(-1, 1), random_float(-1, 1)};
}

void create_sphere(vector<vec3>& verts, vector<vec3>& normals, vector<unsigned int>& faceList, int xpartitions, int ypartitions, float radius, vec3 offset);

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

GLuint create_shader(const char* shaderSource, GLenum shaderType) {
    int success;
    char infoLog[512];

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "shader compilation failed:\n" << infoLog << "\n";
    }

    return shader;
}

GLFWwindow* create_window(int screen_width, int screen_height, std::string screen_name){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(screen_width, screen_height, screen_name.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
    }

    glViewport(0, 0, screen_width, screen_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}

void create_particle_system(vector<vec3>& particles, vector<vec3>& predicted_particles, vector<vec3>& velocities, vector<float>& densities, vec3 min_bound, vec3 max_bound, int rows, int cols, int planes) {
    vec3 extra(5.);

    vec3 bound_size = max_bound - min_bound;
    vec3 spacing((bound_size.x - extra.x) / (float)rows,
                  (bound_size.y - extra.y) / (float)cols,
                  (bound_size.z - extra.z) / (float)planes);

    // make a particle grid with some randomization in how they are placed along
    // the grid
    vec3 pos = min_bound;
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
}