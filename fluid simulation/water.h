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

using std::max;
using std::pow;
using std::round;
using std::sqrt;
using std::unordered_map;
using std::string;

vec3 interpolate(vec3 start, vec3 end, float t) { return (end - start) * t + start; }

void bounds_check(vec4& point, vec4& v, float damping, vec3 min_pos, vec3 max_pos);

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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);


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

GLuint create_shader_program(vector<GLuint>& shaders) {
    int success;
    char infoLog[512];
    GLuint shaderProgram = glCreateProgram();
    for(int i = 0; i < (int)shaders.size(); ++i){
        glAttachShader(shaderProgram, shaders[i]);
    }
    
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "shader program linking failed:\n" << infoLog << "\n";
    }

    for(int i = 0; i < (int)shaders.size(); ++i){
        glDeleteShader(shaders[i]);
    }

    return shaderProgram;
}

GLuint create_shader_program(GLuint shader) {
    int success;
    char infoLog[512];
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, shader);
    
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "shader program linking failed:\n" << infoLog << "\n";
    }

    glDeleteShader(shader);

    return shaderProgram;
}

void create_sphere(vector<vec3>& verts, vector<vec3>& normals, vector<unsigned int>& faceList, int xpartitions, int ypartitions, float radius) {
    for (int i = 0; i < ypartitions + 1; ++i) {
        double u = (2.0 * M_PI * i) / (double)ypartitions;
        for (int j = 0; j < xpartitions + 1; ++j) {
            double v = (M_PI * j) / (xpartitions / 2.0) - (M_PI / 2.0);
            float cu = cos(u);
            float su = sin(u);
            float cv = cos(v);
            float sv = sin(v);
        
            verts.push_back(radius * vec3(cv * cu, cv * su, sv));

            vec3 normal = normalize(radius * vec3(cv * cu, cv * su, sv));
            normals.push_back(normal);

            // normalize
            //normals.push_back(vec3(nx, ny, nz) / std::sqrt(nx * nx + ny * ny + nz * nz));
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

// grid builds in +x, +y, +z from offset where x builds rows, y builds cols, and z builds planes
void create_particle_cube(vector<vec4>& particles, vec3 offset, float size, int rows, int cols, int planes) {
    vec3 spacing(size / (float)rows, size / (float)cols, size / (float)planes);
    vec4 pos = vec4(0.);
    
    for (int i = 0; i < rows; ++i) {
        pos.x = offset.x + i * spacing.x;
        for (int j = 0; j < cols; ++j) {
            pos.y = offset.y + j * spacing.y;
            for (int k = 0; k < planes; ++k) {
                pos.z = offset.z + k * spacing.z;

                float rx = random_float(-spacing.x / 4., spacing.x / 4.);
                float ry = random_float(-spacing.y / 4., spacing.y / 4.);
                float rz = random_float(-spacing.z / 4., spacing.z / 4.);
                
                particles.push_back(pos + vec4(rx, ry, rz, 0.));
            }
        }
    }
}

// used to create a t value for interpolate ie a value between 0-1
float scale_t_val(float value, float data_min, float data_max) {
    return (value - data_min) / (data_max - data_min);
}