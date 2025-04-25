#include <stdlib.h>
#include <time.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::pow;
using std::string;
using std::to_string;
using std::vector;

struct V3 {
    float x = 0;
    float y = 0;
    float z = 0;

    V3 operator+(V3 coord) {
        V3 ans;
        ans.x = this->x + coord.x;
        ans.y = this->y + coord.y;
        ans.z = this->z + coord.z;
        return ans;
    }

    void operator+=(V3 coord) {
        this->x += coord.x;
        this->y += coord.y;
        this->z += coord.z;
    }

    void operator*=(V3 coord) {
        this->x *= coord.x;
        this->y *= coord.y;
        this->z *= coord.z;
    }

    V3 operator+(float c) {
        V3 ans;
        ans.x = this->x + c;
        ans.y = this->y + c;
        ans.z = this->z + c;
        return ans;
    }
    V3 operator-(V3 coord) {
        V3 ans;
        ans.y = this->y - coord.y;
        ans.z = this->z - coord.z;
        ans.x = this->x - coord.x;
        return ans;
    }
    V3 operator*(V3 coord) {
        V3 ans;
        ans.x = this->x * coord.x;
        ans.y = this->y * coord.y;
        ans.z = this->z * coord.z;
        return ans;
    }
    V3 operator/(V3 coord) {
        V3 ans;
        ans.x = this->x / coord.x;
        ans.y = this->y / coord.y;
        ans.z = this->z / coord.z;
        return ans;
    }
    V3 operator*(float c) {
        V3 ans;
        ans.x = this->x * c;
        ans.y = this->y * c;
        ans.z = this->z * c;
        return ans;
    }
    V3 operator/(float c) {
        V3 ans;
        ans.x = this->x / c;
        ans.y = this->y / c;
        ans.z = this->z / c;
        return ans;
    }

    void p() {
        cout << "x:" << this->x << " y:" << this->y << " z:" << this->z << endl;
    }
};

void print_vec(vector<vector<int>>& X);

float magnitude(V3 v);

V3 interpolate(V3 start, V3 end, double t);

double scale_t_val(double value, double data_min, double data_max);