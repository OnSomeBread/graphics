#include <cmath>

#include "cs630.h"
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
};

struct VN {
    vector<float> attrs;
    VN operator+(VN v2) {
        VN ans;
        int s = std::max(this->attrs.size(), v2.attrs.size());
        for (int i = 0; i < s; ++i) {
            float val = 0;
            if (i >= this->attrs.size()) {
                val = v2.attrs[i];
            } else if (i >= v2.attrs.size()) {
                val = this->attrs[i];
            } else {
                val = this->attrs[i] + v2.attrs[i];
            }
            ans.attrs.push_back(val);
        }
        return ans;
    }

    VN operator+(float c) {
        VN ans;
        for (int i = 0; i < (int)this->attrs.size(); ++i) {
            ans.attrs.push_back(this->attrs[i] + c);
        }
        return ans;
    }

    VN operator*(VN v2) {
        VN ans;
        int s = std::max(this->attrs.size(), v2.attrs.size());
        for (int i = 0; i < s; ++i) {
            float val = 0;
            if (i >= this->attrs.size()) {
                val = v2.attrs[i];
            } else if (i >= v2.attrs.size()) {
                val = this->attrs[i];
            } else {
                val = this->attrs[i] * v2.attrs[i];
            }
            ans.attrs.push_back(val);
        }
        return ans;
    }

    VN operator*(float c) {
        VN ans;
        for (int i = 0; i < (int)this->attrs.size(); ++i) {
            ans.attrs.push_back(this->attrs[i] * c);
        }
        return ans;
    }

    void operator=(VN v2) {
        int s = std::max(this->attrs.size(), v2.attrs.size());
        this->attrs.resize(s, 0);
        for (int i = 0; i < s; ++i) {
            this->attrs[i] = v2.attrs[i];
        }
    }

    V3 get_coord() {
        V3 ans;
        ans.x = this->attrs[0];
        ans.y = this->attrs[1];
        ans.z = this->attrs[2];
        return ans;
    }
};