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