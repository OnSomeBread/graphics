#include <cmath>
#include <iostream>

#include "cs630.h"

using std::cout;
using std::endl;

struct V3 {
    float x;
    float y;
    float z;

    V3 operator+(V3 coord) {
        V3 ans;
        ans.x = this->x + coord.x;
        ans.y = this->y + coord.y;
        ans.z = this->z + coord.z;
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
    V3 operator*(float c) {
        V3 ans;
        ans.x = this->x * c;
        ans.y = this->y * c;
        ans.z = this->z * c;
        return ans;
    }
};

V3 decastlejau(vector<V3> &coords, float t) {
    int n = coords.size();
    while (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            coords[i] = coords[i] + (coords[i + 1] - coords[i]) * t;
        }
    }

    return coords[0];
}

int render_direct::render_bezier_curve(const string &vertex_type, int degree,
                                       const vector<float> &vertex) {
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    vector<V3> coords;
    for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
        V3 coord;
        coord.x = vertex[i + data_m_attr.geometry];
        coord.y = vertex[i + data_m_attr.geometry + 1];
        coord.z = vertex[i + data_m_attr.geometry + 2];
        coords.push_back(coord);
    }

    if (coords.size() == 0) {
        return 0;
    }

    // V3 v1 = coords[0];
    // V3 v2 = coords[coords.size() - 1];
    // float dist = std::sqrt((v2.x - v1.x) * (v2.x - v1.x) +
    //                        (v2.y - v1.y) * (v2.y - v1.y) +
    //                        (v2.z - v1.z) * (v2.z - v1.z));
    // float total = n_divisions
    float t = 0;  // t = 0 is coords[0] and t = 1 is coords[coords.size() - 1]
    float interval_size = 1.0 / n_divisions;
    vector<V3> newcoords;
    while (t < 1) {
        newcoords.push_back(decastlejau(coords, t));
        t += interval_size;
    }

    render_m_attr.add_shading_offset();

    return 0;
}

int render_direct::render_bezier_patch(const string &vertex_type, int u_degree,
                                       int v_degree,
                                       const vector<float> &vertex) {
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    return 0;
}

int REDirect::rd_sqsphere(float radius, float north, float east, float zmin,
                          float zmax, float thetamax) {
    return 0;
}

int REDirect::rd_sqtorus(float radius1, float radius2, float north, float east,
                         float phimin, float phimax, float thetamax) {
    return 0;
}