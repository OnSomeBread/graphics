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
    V3 operator*(float c) {
        V3 ans;
        ans.x = this->x * c;
        ans.y = this->y * c;
        ans.z = this->z * c;
        return ans;
    }
};

struct Vcolor {
    float r;
    float g;
    float b;
};

vector<V3> get_coords(const vector<float> &vertex) {
    vector<V3> coords;
    if (data_m_attr.geom_flag) {
        for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
            V3 coord;
            coord.x = vertex[i + data_m_attr.geometry];
            coord.y = vertex[i + data_m_attr.geometry + 1];
            coord.z = vertex[i + data_m_attr.geometry + 2];
            coords.push_back(coord);
        }
    }
    return coords;
}

vector<Vcolor> get_colors(const vector<float> &vertex) {
    vector<Vcolor> colors;
    if (data_m_attr.color_flag) {
        for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
            Vcolor color;
            color.r = vertex[i + data_m_attr.color];
            color.g = vertex[i + data_m_attr.color + 1];
            color.b = vertex[i + data_m_attr.color + 2];
            colors.push_back(color);
        }
    }
    return colors;
}

// V3 decastlejau(vector<V3> &coords, float t) {
//     int n = coords.size();
//     while (n > 1) {
//         for (int i = 0; i < n - 1; ++i) {
//             coords[i] = coords[i] + (coords[i + 1] - coords[i]) * t;
//         }
//         n -= 1;
//     }

//     return coords[0];
// }

int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// https://www.scratchapixel.com/lessons/geometry/bezier-curve-rendering-utah-teapot/bezier-surface.html
V3 evalBezierCurve(vector<V3> &coords, float t) {
    V3 ans;
    ans.x = 0;
    ans.y = 0;
    ans.z = 0;
    int n = coords.size() - 1;
    for (int k = 0; k <= n; ++k) {
        float coef = factorial(n) / (factorial(k) * factorial(n - k));
        float binomal = coef * std::pow(1 - t, n - k) * std::pow(t, k);
        ans = ans + (coords[k] * binomal);
    }
    return ans;
}

int render_direct::render_bezier_curve(const string &vertex_type, int degree,
                                       const vector<float> &vertex) {
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    vector<V3> coords = get_coords(vertex);
    if (coords.size() == 0) {
        return 0;
    }

    vector<Vcolor> colors = get_colors(vertex);

    float t = 0;  // t = 0 is coords[0] and t = 1 is coords[coords.size() - 1]
    float interval_size = 1.0 / n_divisions;
    vector<V3> new_coords;
    while (t < 1) {
        new_coords.push_back(evalBezierCurve(coords, t));
        t += interval_size;
    }

    for (int i = 0; i < (int)new_coords.size() - 1; ++i) {
        attr_point s;
        s.coord[0] = new_coords[i].x;
        s.coord[1] = new_coords[i].y;
        s.coord[2] = new_coords[i].z;
        s.coord[3] = 1.0;  // required
        s.coord[4] = 1.0;  // required

        // color ????
        // s.coord[5] = 0.5;
        // s.coord[6] = 0.5;
        // s.coord[7] = 0.5;

        attr_point e;
        e.coord[0] = new_coords[i + 1].x;
        e.coord[1] = new_coords[i + 1].y;
        e.coord[2] = new_coords[i + 1].z;
        e.coord[3] = 1.0;
        e.coord[4] = 1.0;

        line_pipeline(s, MOVE);
        line_pipeline(e, DRAW);
    }

    render_m_attr.add_shading_offset();

    return 0;
}

// BEZIER PATCH
int render_direct::render_bezier_patch(const string &vertex_type, int u_degree,
                                       int v_degree,
                                       const vector<float> &vertex) {
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    vector<V3> pre_coords = get_coords(vertex);
    if (pre_coords.size() == 0) {
        return 0;
    }

    vector<Vcolor> colors = get_colors(vertex);

    // NEEDED FOR UV OPTIMIZATION
    // vector<vector<V3> > coords;
    // for (int i = 0; i < u_degree + 1; ++i) {
    //     vector<V3> c;
    //     for (int j = 0; j < v_degree + 1; ++j) {
    //         c.push_back(pre_coords[i * (v_degree + 1) + j]);
    //     }
    //     coords.push_back(c);
    // }

    vector<vector<V3> > new_coords;
    float interval_size = 1.0 / n_divisions;
    float u = 0;
    while (u < 1) {
        float v = 0;

        vector<V3> new_coords_rows;

        // FIND A BETTER WAY TO HANDLE UCOORDS AND VCOORDS
        vector<V3> ucoords;
        for (int i = 0; i < pre_coords.size(); ++i) {
            ucoords.push_back(pre_coords[i]);
        }

        V3 up = evalBezierCurve(ucoords, u);
        while (v < 1) {
            vector<V3> vcoords;
            for (int i = 0; i < pre_coords.size(); ++i) {
                vcoords.push_back(pre_coords[i]);
            }
            new_coords_rows.push_back(evalBezierCurve(vcoords, v) * up);
            v += interval_size;
        }
        new_coords.push_back(new_coords_rows);
        u += interval_size;
    }

    // draw the bezier patch
    for (int i = 0; i < (int)new_coords.size() - 1; ++i) {
        for (int j = 0; j < (int)new_coords[i].size() - 1; j++) {
            vector<V3> points;
            points.push_back(new_coords[i][j]);
            points.push_back(new_coords[i + 1][j]);
            points.push_back(new_coords[i][j + 1]);
            points.push_back(new_coords[i + 1][j + 1]);

            vector<attr_point> attrs;
            for (int k = 0; k < (int)points.size(); ++k) {
                attr_point a;
                a.coord[0] = points[i].x;
                a.coord[1] = points[i].y;
                a.coord[2] = points[i].z;
                a.coord[3] = 1.0;
                a.coord[4] = 1.0;
                attrs.push_back(a);
            }

            poly_pipeline(attrs[0], MOVE);
            poly_pipeline(attrs[2], MOVE);
            poly_pipeline(attrs[3], MOVE);
            poly_pipeline(attrs[1], DRAW);
        }
    }

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