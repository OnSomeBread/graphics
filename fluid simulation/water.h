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

void create_sphere(vector<vec3>& verts, vector<vec3>& normals, vector<unsigned int>& faceList, int xpartitions, int ypartitions, float radius);
void create_particle_cube(vector<vec4>& particles, vec3 offset, float size, int rows, int cols, int planes);
std::string loadShaderSource(const char* filepath);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLuint create_shader(const char* shaderSource, GLenum shaderType);
GLFWwindow* create_window(int screen_width, int screen_height, std::string screen_name);
GLuint create_shader_program(vector<GLuint>& shaders);
GLuint create_shader_program(GLuint shader);
