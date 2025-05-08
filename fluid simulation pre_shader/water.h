#include <iostream>

using std::cout;
using std::endl;

struct V3 {
    double x = 0;
    double y = 0;
    double z = 0;

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

V3 interpolate(V3 start, V3 end, float t) { return (end - start) * t + start; }

void bounds_check(V3& point, V3& v, float damping, V3 min_pos, V3 max_pos);

float random_float(float low, float high) {
    return low + static_cast<float>(rand()) /
                     (static_cast<float>(RAND_MAX / (high - low)));
}

V3 random_dir() {
    return {(float)(rand() % 3 - 1), (float)(rand() % 3 - 1),
            (float)(rand() % 3 - 1)};
}

V3 random_dir_float() {
    return {random_float(-1, 1), random_float(-1, 1), random_float(-1, 1)};
}