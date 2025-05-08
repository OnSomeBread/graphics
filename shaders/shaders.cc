#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using std::vector;
using glm::vec3;
using std::cout;
using std::endl;

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

void create_sphere(vector<vec3>& verts, vector<vec3>& normals, vector<int>& faceList, int xpartitions, int ypartitions, float radius, float offx, float offy, float offz) {
    for (int i = 0; i < ypartitions + 1; ++i) {
        double u = (2.0 * M_PI * i) / (double)ypartitions;
        for (int j = 0; j < xpartitions + 1; ++j) {
            double v = (M_PI * j) / (xpartitions / 2.0) - (M_PI / 2.0);
        
            verts.push_back(radius * vec3(cos(v) * cos(u) + offx, cos(v) * sin(u) + offy, sin(v) + offz));

            // haha normal time
            // partial derivative with respect to u
            vec3 du(cos(v) * -sin(u), cos(v) * cos(u), 0);
            du *= radius;

            // partial derivative with respect to v
            vec3 dv(-sin(v) * cos(u), -sin(v) * sin(u), cos(v));
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
    for (int y = 0; y < ypartitions; ++y) {
        for (int x = 0; x < xpartitions; ++x) {
            int i1 = y * (xpartitions + 1) + x;
            int i2 = (y + 1) * (xpartitions + 1) + x;
    
            faceList.push_back(i1);
            faceList.push_back(i2);
            faceList.push_back(i1 + 1);
    
            faceList.push_back(i1 + 1);
            faceList.push_back(i2);
            faceList.push_back(i2 + 1);
        }
    }
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

    std::string vertexShaderStr = loadShaderSource("vertexShaderArt1.glsl");
    std::string fragmentShaderStr = loadShaderSource("fragmentShaderArt2.glsl");
    
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

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = vertexShaderStr.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Compilation Failed:\n" << infoLog << "\n";
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = fragmentShaderStr.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Compilation Failed:\n" << infoLog << "\n";
    }

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

    float vertices[] = {-1.0f, -1.0f, 0.0f,  
                        1.0f, -1.0f, 0.0f, 
                        -1.0f, 1.0f, 0.0f,
                        1.0f, 1.0f, 0.0f
                        };


    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    int frames = 0;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        GLuint resLoc = glGetUniformLocation(shaderProgram, "u_resolution");
        glUniform2f(resLoc, (float)screen_width, (float)screen_height);

        GLuint timeLoc = glGetUniformLocation(shaderProgram, "u_time");
        glUniform1f(timeLoc, (float)glfwGetTime());

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

        frames++;

        //std::cout << (int)(frames / glfwGetTime()) << std::endl;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
