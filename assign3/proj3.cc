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

// struct Vcolor {
//     float r;
//     float g;
//     float b;
// };

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

vector<V3> get_colors(const vector<float> &vertex) {
    vector<V3> colors;
    if (data_m_attr.color_flag) {
        for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
            V3 color;
            color.x = vertex[i + data_m_attr.color];
            color.y = vertex[i + data_m_attr.color + 1];
            color.z = vertex[i + data_m_attr.color + 2];
            colors.push_back(color);
        }
    }
    return colors;
}

int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

V3 eval_bezier_curve(vector<V3> &coords, float t) {
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

V3 eval_bezier_patch(vector<vector<V3>> &coords, float u, float v) {
    V3 ans;
    ans.x = 0;
    ans.y = 0;
    ans.z = 0;
    int m = coords.size() - 1;
    int n = coords[0].size() - 1;

    for (int i = 0; i <= m; ++i) {
        float ucoef = factorial(m) / (factorial(i) * factorial(m - i));
        float ubinomal = ucoef * std::pow(1 - u, m - i) * std::pow(u, i);

        for (int j = 0; j <= n; ++j) {
            float vcoef = factorial(n) / (factorial(j) * factorial(n - j));
            float vbinomal = vcoef * std::pow(1 - v, n - j) * std::pow(v, j);

            ans = ans + (coords[i][j] * ubinomal * vbinomal);
        }
    }

    return ans;
}

float deriv_product_rule(int n, int k, float t) {
    float left_product_rule =
        -(n - k) * std::pow(1 - t, n - k - 1) * std::pow(t, k);
    float right_product_rule = k * std::pow(t, k - 1) * std::pow(1 - t, n - k);
    return left_product_rule + right_product_rule;
}

V3 du_bezier_patch(vector<vector<V3>> &coords, float u, float v) {
    V3 ans;
    ans.x = 0;
    ans.y = 0;
    ans.z = 0;
    int m = coords.size() - 1;
    int n = coords[0].size() - 1;

    for (int i = 0; i <= m; ++i) {
        float ucoef = factorial(m) / (factorial(i) * factorial(m - i));
        // float ubinomal = ucoef * std::pow(1 - u, m - i) * std::pow(u, i);
        float dubinomal = ucoef * deriv_product_rule(m, i, u);

        for (int j = 0; j <= n; ++j) {
            float vcoef = factorial(n) / (factorial(j) * factorial(n - j));
            float vbinomal = vcoef * std::pow(1 - v, n - j) * std::pow(v, j);

            ans = ans + (coords[i][j] * dubinomal * vbinomal);
        }
    }

    return ans;
}

V3 dv_bezier_patch(vector<vector<V3>> &coords, float u, float v) {
    V3 ans;
    ans.x = 0;
    ans.y = 0;
    ans.z = 0;
    int m = coords.size() - 1;
    int n = coords[0].size() - 1;

    for (int i = 0; i <= m; ++i) {
        float ucoef = factorial(m) / (factorial(i) * factorial(m - i));
        float ubinomal = ucoef * std::pow(1 - u, m - i) * std::pow(u, i);

        for (int j = 0; j <= n; ++j) {
            float vcoef = factorial(n) / (factorial(j) * factorial(n - j));
            // float vbinomal = vcoef * std::pow(1 - v, n - j) * std::pow(v, j);
            float dvbinomal = vcoef * deriv_product_rule(n, j, v);

            ans = ans + (coords[i][j] * ubinomal * dvbinomal);
        }
    }

    return ans;
}

V3 cross_product(V3 a, V3 b) {
    V3 ans;
    ans.x = a.y * b.z - a.z * b.y;
    ans.y = a.z * b.x - a.x * b.z;
    ans.z = a.x * b.y - a.y * b.x;
    // ans.x = a.y * b.z - a.z * b.y;
    // ans.y = -(a.x * b.z - a.z * b.x);
    // ans.z = a.x * b.y - a.y * b.x;
    return ans;
}

V3 normalize(V3 v) { return v / -std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

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

    vector<V3> colors = get_colors(vertex);
    if (colors.size() != 0) {
        render_m_attr.add_color();
    }

    float t = 0;  // t = 0 is coords[0] and t = 1 is coords[coords.size() - 1]
    float interval_size = 1.0 / (n_divisions + 1);
    vector<V3> new_coords;
    vector<V3> new_colors;
    while (t < 1) {
        new_coords.push_back(eval_bezier_curve(coords, t));
        new_colors.push_back(eval_bezier_curve(colors, t));
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
        // s.coord[render_m_attr.color] = 1;
        // s.coord[render_m_attr.color + 1] = 1;
        // s.coord[render_m_attr.color + 2] = 1;
        // s.coord[5] = 1;
        // s.coord[6] = 1;
        // s.coord[7] = 1;
        // s.coord[8] = 1;
        // s.coord[9] = 1;
        // s.coord[10] = 1;

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

    vector<V3> colors = get_colors(vertex);
    if (colors.size() != 0) {
        render_m_attr.add_color();
    }

    vector<vector<V3>> coords;
    for (int i = 0; i < v_degree + 1; ++i) {
        vector<V3> c;
        for (int j = 0; j < u_degree + 1; ++j) {
            c.push_back(pre_coords[i * (u_degree + 1) + j]);
        }
        coords.push_back(c);
    }

    vector<vector<V3>> new_coords;
    vector<vector<V3>> new_coords_normals;
    float interval_size = 1.0 / (n_divisions + 1);
    float u = 0;
    while (u < 1) {
        vector<V3> new_coords_rows;
        vector<V3> new_coords_rows_normals;
        float v = 0;
        while (v < 1) {
            new_coords_rows.push_back(eval_bezier_patch(coords, u, v));
            new_coords_rows_normals.push_back(normalize(cross_product(
                du_bezier_patch(coords, u, v), dv_bezier_patch(coords, u, v))));

            v += interval_size;
        }

        new_coords.push_back(new_coords_rows);
        new_coords_normals.push_back(new_coords_rows_normals);

        u += interval_size;
    }

    // draw the bezier patch
    for (int i = 0; i < (int)new_coords.size() - 1; ++i) {
        for (int j = 0; j < (int)new_coords[i].size() - 1; ++j) {
            vector<V3> points;
            points.push_back(new_coords[i][j]);
            points.push_back(new_coords[i + 1][j]);
            points.push_back(new_coords[i][j + 1]);
            points.push_back(new_coords[i + 1][j + 1]);

            vector<attr_point> attrs;
            for (int k = 0; k < (int)points.size(); ++k) {
                attr_point a;
                a.coord[0] = points[k].x;
                a.coord[1] = points[k].y;
                a.coord[2] = points[k].z;
                a.coord[3] = 1.0;
                a.coord[4] = 1.0;

                attrs.push_back(a);
            }

            poly_normal[0] = new_coords_normals[i][j].x;
            poly_normal[1] = new_coords_normals[i][j].y;
            poly_normal[2] = new_coords_normals[i][j].z;

            // CC dir
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