#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

using std::vector;

void create_sphere(vector<float>& verts, vector<float>& normals, vector<int>& faceList, int xpartitions, int ypartitions, float radius, float offx, float offy, float offz) {
    for (int i = 0; i < ypartitions + 1; ++i) {
        double u = (2.0 * M_PI * i) / (double)ypartitions;
        for (int j = 0; j < xpartitions + 1; ++j) {
            double v = (M_PI * j) / (xpartitions / 2.0) - (M_PI / 2.0);
        
            verts.push_back(radius * cos(v) * cos(u) + offx);
            verts.push_back(radius * cos(v) * sin(u) + offy);
            verts.push_back(radius * sin(v) + offz);

            // haha normal time
            // partial derivative with respect to u
            float dux = radius * cos(v) * -sin(u);
            float duy = radius * cos(v) * cos(u);
            float duz = 0;

            // partial derivative with respect to v
            float dvx = radius * -sin(v) * cos(u);
            float dvy = radius * -sin(v) * sin(u);
            float dvz = radius * cos(v);

            // du X dv
            float nx = duy * dvz - duz * dvy;
            float ny = duz * dvx - dux * dvz;
            float nz = dux * dvy - duy * dvx;

            // normalize
            float mag = std::sqrt(nx * nx + ny * ny + nz * nz);
            normals.push_back(nx / mag);
            normals.push_back(ny / mag);
            normals.push_back(nz / mag);
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

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 410 core

    uniform vec2 iResolution;
    uniform float iTime;

    out vec4 FragColor;

    vec3 palette(float t){
        vec3 a = vec3(.5);
        vec3 b = vec3(.5);
        vec3 c = vec3(1.);
        //vec3 d = vec3(.263, .416, .557);
        vec3 d = vec3(0.30, 0.20, 0.20);

        return a + b*cos( 6.283185*(c*t+d) );
    }

    void main() {
        vec2 uv = gl_FragCoord.xy / iResolution.xy * 2.0 - 1.0;
        uv.x *= iResolution.x / iResolution.y;

        vec3 finalColor = vec3(0.);

        vec2 uv0 = uv;
        for(int i = 0; i < 6; ++i){
            uv = fract(uv * 1.5) - .5;

            float d = length(uv) * exp(-length(uv0));

            vec3 col = palette(length(uv0) + i*.4 + iTime*.4);

            d = sin(d*8. + iTime * .8)/8.;
            d = pow(.01 / abs(d), 1.2);
            
            finalColor += (col * d) / 4.;
        }
        FragColor = vec4(finalColor, 1.0);
    }
)glsl";

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

        // Dark background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        GLuint resLoc = glGetUniformLocation(shaderProgram, "iResolution");
        glUniform2f(resLoc, (float)screen_width, (float)screen_height);

        GLuint timeLoc = glGetUniformLocation(shaderProgram, "iTime");
        glUniform1f(timeLoc, (float)glfwGetTime());

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

        frames++;

        std::cout << (int)(frames / glfwGetTime()) << std::endl;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
