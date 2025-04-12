#include <cmath>
#include <iostream>

#include "cs630.h"

using std::cout;
using std::endl;

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

// used by both bezier curves and patches to grab xyz coords from user
vector<V3> get_coords(const vector<float> &vertex) {
    vector<V3> coords;
    for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
        V3 coord;
        coord.x = vertex[i + data_m_attr.geometry];
        coord.y = vertex[i + data_m_attr.geometry + 1];
        coord.z = vertex[i + data_m_attr.geometry + 2];
        coords.push_back(coord);
    }
    return coords;
}

// used by both bezier curves and patches to grab colors from the user for each
// point
vector<V3> get_colors(const vector<float> &vertex) {
    vector<V3> colors;
    for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
        V3 color;
        color.x = vertex[i + data_m_attr.color];
        color.y = vertex[i + data_m_attr.color + 1];
        color.z = vertex[i + data_m_attr.color + 2];
        colors.push_back(color);
    }
    return colors;
}

// used by both bezier curves and patches for weighted values and opacity
vector<V3> get_wp(const vector<float> &vertex) {
    vector<V3> wp;
    for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
        V3 v;
        v.x = vertex[i + data_m_attr.weight];
        v.y = vertex[i + data_m_attr.opacity];
        wp.push_back(v);
    }
    return wp;
}

// used by both bezier curves and patches for texture values
vector<V3> get_textures(const vector<float> &vertex) {
    vector<V3> texture;
    for (int i = 0; i < (int)vertex.size(); i += data_m_attr.size) {
        V3 v;
        v.x = vertex[i + data_m_attr.texture];
        v.y = vertex[i + data_m_attr.texture + 1];
        texture.push_back(v);
    }
    return texture;
}

// recursive n_choose_k
float n_choose_k(int n, int k) {
    if (k == 0) return 1;
    return (n * n_choose_k(n - 1, k - 1)) / k;
}

// the bernstein basis polynomials calculation
float bernstein(int n, int k, float t) {
    return n_choose_k(n, k) * std::pow(1 - t, n - k) * std::pow(t, k);
}

// bernstein calculation of bezuer curve for a given t
// uses all control points
V3 eval_bezier_curve(vector<V3> &coords, float t) {
    V3 ans;
    int n = coords.size() - 1;
    for (int k = 0; k <= n; ++k) {
        ans = ans + (coords[k] * bernstein(n, k, t));
    }
    return ans;
}

// bernstein calculation of bezier patch for particular u and v
// uses all control points
V3 eval_bezier_patch(vector<vector<V3>> &coords, float u, float v) {
    V3 ans;
    int m = coords.size() - 1;
    int n = coords[0].size() - 1;

    for (int i = 0; i <= m; ++i) {
        float ubinomal = bernstein(m, i, u);
        for (int j = 0; j <= n; ++j) {
            ans = ans + (coords[i][j] * ubinomal * bernstein(n, j, v));
        }
    }

    return ans;
}

// function that both partials use to calculate the derivitive
float deriv_bernstein(int n, int k, float t) {
    // left side = (1 - t)^(n - k)
    float left = -(n - k) * std::pow(1 - t, n - k - 1) * std::pow(t, k);
    // right side = t^k
    float right = k * std::pow(t, k - 1) * std::pow(1 - t, n - k);

    // YES ALL IF STATEMENTS HERE ARE NECESSARY
    if (std::isnan(left) && std::isnan(right)) {
        return 0;
    }
    if (std::isnan(left)) {
        return n_choose_k(n, k) * right;
    }
    if (std::isnan(right)) {
        return n_choose_k(n, k) * left;
    }
    return n_choose_k(n, k) * (left + right);
}

// partial derivative with respect to u of bezier patch calculation
V3 du_bezier_patch(vector<vector<V3>> &coords, float u, float v) {
    V3 ans;
    int m = coords.size() - 1;
    int n = coords[0].size() - 1;

    for (int i = 0; i <= m; ++i) {
        float dubinomal = deriv_bernstein(m, i, u);
        for (int j = 0; j <= n; ++j) {
            ans = ans + (coords[i][j] * dubinomal * bernstein(n, j, v));
        }
    }

    return ans;
}

// partial derivative with respect to v of bezier patch calculation
V3 dv_bezier_patch(vector<vector<V3>> &coords, float u, float v) {
    V3 ans;
    int m = coords.size() - 1;
    int n = coords[0].size() - 1;

    for (int i = 0; i <= m; ++i) {
        float ubinomal = bernstein(m, i, u);
        for (int j = 0; j <= n; ++j) {
            ans = ans + (coords[i][j] * ubinomal * deriv_bernstein(n, j, v));
        }
    }

    return ans;
}

// vector1 X vector2
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

// given any number of normal vectors returns the average of them all
V3 avg_normal_vectors(vector<V3> &norms) {
    V3 avg;
    for (int i = 0; i < (int)norms.size(); ++i) {
        avg = avg + norms[i];
    }
    return normalize(avg / -(int)norms.size());
}

namespace render_direct {

int render_bezier_curve(const string &vertex_type, int degree,
                        const vector<float> &vertex) {
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    if (!data_m_attr.geom_flag) {
        return 1;
    }

    vector<V3> coords = get_coords(vertex);
    if (coords.size() == 0) {
        return 1;
    }

    vector<V3> colors = get_colors(vertex);
    if (render_m_attr.color_flag) {
        render_m_attr.add_color();
    }

    vector<V3> wps = get_wp(vertex);
    if (render_m_attr.weight_flag) {
        render_m_attr.add_weight();
    }
    if (render_m_attr.opacity_flag) {
        render_m_attr.add_opacity();
    }

    vector<V3> textures;
    if (render_m_attr.texture_flag) {
        textures = get_textures(vertex);
        render_m_attr.add_texture();
    }

    render_m_attr.add_shading_offset();

    vector<V3> new_coords;
    vector<V3> new_colors;
    vector<V3> new_wps;
    vector<V3> new_textures;
    for (int i = 0; i < n_divisions + 1; ++i) {
        float t = i / (float)n_divisions;
        new_coords.push_back(eval_bezier_curve(coords, t));
        new_colors.push_back(eval_bezier_curve(colors, t));
        new_wps.push_back(eval_bezier_curve(wps, t));
        new_textures.push_back(eval_bezier_curve(textures, t));
    }

    attr_point start;
    start.coord[0] = new_coords[0].x;
    start.coord[1] = new_coords[0].y;
    start.coord[2] = new_coords[0].z;
    start.coord[3] = 1.0;
    start.coord[4] = 1.0;

    if (render_m_attr.geom_flag) {
        start.coord[render_m_attr.geometry] = new_coords[0].x;
        start.coord[render_m_attr.geometry + 1] = new_coords[0].y;
        start.coord[render_m_attr.geometry + 2] = new_coords[0].z;
    }

    if (render_m_attr.color_flag) {
        start.coord[render_m_attr.color] = new_colors[0].x;
        start.coord[render_m_attr.color + 1] = new_colors[0].y;
        start.coord[render_m_attr.color + 2] = new_colors[0].z;
    }
    if (render_m_attr.weight_flag) {
        start.coord[render_m_attr.weight] = new_wps[0].x;
    }
    if (render_m_attr.opacity_flag) {
        start.coord[render_m_attr.opacity] = new_wps[0].y;
    }
    if (render_m_attr.texture_flag) {
        start.coord[render_m_attr.texture] = new_textures[0].x;
        start.coord[render_m_attr.texture + 1] = new_textures[0].y;
    }
    line_pipeline(start, MOVE);

    for (int i = 1; i < (int)new_coords.size(); ++i) {
        attr_point s;
        s.coord[0] = new_coords[i].x;
        s.coord[1] = new_coords[i].y;
        s.coord[2] = new_coords[i].z;
        s.coord[3] = 1.0;
        s.coord[4] = 1.0;

        if (render_m_attr.geom_flag) {
            s.coord[render_m_attr.geometry] = new_coords[i].x;
            s.coord[render_m_attr.geometry + 1] = new_coords[i].y;
            s.coord[render_m_attr.geometry + 2] = new_coords[i].z;
        }
        if (render_m_attr.color_flag) {
            s.coord[render_m_attr.color] = new_colors[i].x;
            s.coord[render_m_attr.color + 1] = new_colors[i].y;
            s.coord[render_m_attr.color + 2] = new_colors[i].z;
        }
        if (render_m_attr.weight_flag) {
            s.coord[render_m_attr.weight] = new_wps[i].x;
        }
        if (render_m_attr.opacity_flag) {
            s.coord[render_m_attr.opacity] = new_wps[i].y;
        }
        if (render_m_attr.texture_flag) {
            s.coord[render_m_attr.texture] = new_textures[i].x;
            s.coord[render_m_attr.texture + 1] = new_textures[i].y;
        }

        line_pipeline(s, DRAW);
    }

    return 0;
}

// BEZIER PATCH
int render_bezier_patch(const string &vertex_type, int u_degree, int v_degree,
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

    vector<V3> pre_colors = get_colors(vertex);
    if (render_m_attr.color_flag) {
        render_m_attr.add_color();
    }

    vector<V3> pre_wps = get_wp(vertex);
    if (render_m_attr.weight_flag) {
        render_m_attr.add_weight();
    }
    if (render_m_attr.opacity_flag) {
        render_m_attr.add_opacity();
    }

    vector<V3> pre_textures = get_textures(vertex);
    if (render_m_attr.texture_flag) {
        render_m_attr.add_texture();
    }

    render_m_attr.add_normal();

    render_m_attr.add_shading_offset();

    vector<vector<V3>> coords;
    vector<vector<V3>> colors;
    vector<vector<V3>> wps;
    vector<vector<V3>> textures;
    // put the coords into v_degree + 1 by u_degree + 1 vector for calculating
    for (int i = 0; i < v_degree + 1; ++i) {
        vector<V3> c;
        vector<V3> c2;
        vector<V3> c3;
        vector<V3> c4;
        for (int j = 0; j < u_degree + 1; ++j) {
            c.push_back(pre_coords[i * (u_degree + 1) + j]);
            if (render_m_attr.color_flag) {
                c2.push_back(pre_colors[i * (u_degree + 1) + j]);
            }
            if (render_m_attr.weight_flag || render_m_attr.opacity_flag) {
                c3.push_back(pre_wps[i * (u_degree + 1) + j]);
            }
            if (render_m_attr.texture_flag) {
                c4.push_back(pre_textures[i * (u_degree + 1) + j]);
            }
        }
        coords.push_back(c);
        colors.push_back(c2);
        wps.push_back(c3);
        textures.push_back(c4);
    }

    vector<vector<V3>> patch_coords;
    vector<vector<V3>> patch_normals;
    vector<vector<V3>> patch_colors;
    vector<vector<V3>> patch_wps;
    vector<vector<V3>> patch_textures;

    for (int i = 0; i < n_divisions + 1; ++i) {
        float u = i / (float)n_divisions;
        vector<V3> ncr;
        vector<V3> ncr_normals;
        vector<V3> ncr_colors;
        vector<V3> ncr_wps;
        vector<V3> ncr_textures;
        float v = 0;
        for (int j = 0; j < n_divisions + 1; ++j) {
            v = j / (float)n_divisions;
            ncr.push_back(eval_bezier_patch(coords, u, v));
            // calculate the normal at same point
            // normalize(cross_product(partialdu, partialdv))
            if (render_m_attr.normal_flag) {
                ncr_normals.push_back(
                    normalize(cross_product(du_bezier_patch(coords, u, v),
                                            dv_bezier_patch(coords, u, v))));
            }

            // calculated the same way as geometric coords
            if (render_m_attr.color_flag) {
                ncr_colors.push_back(eval_bezier_patch(colors, u, v));
            }

            // calculated the same way as geometric coords
            if (render_m_attr.weight_flag || render_m_attr.opacity_flag) {
                ncr_wps.push_back(eval_bezier_patch(wps, u, v));
            }

            // calculated the same way as geometric coords
            if (render_m_attr.texture_flag) {
                ncr_textures.push_back(eval_bezier_patch(textures, u, v));
            }
        }

        patch_coords.push_back(ncr);
        patch_normals.push_back(ncr_normals);
        patch_colors.push_back(ncr_colors);
        patch_wps.push_back(ncr_wps);
        patch_textures.push_back(ncr_textures);
    }

    // draw the bezier patch
    for (int i = 0; i < (int)patch_coords.size(); ++i) {
        for (int j = 0; j < (int)patch_coords[i].size(); ++j) {
            // a new way to handle more flags
            vector<int> di = {0};
            vector<int> dj = {0};
            if (j != patch_coords[i].size() - 1) {
                di.push_back(0);
                dj.push_back(1);
            }
            if (i != patch_coords.size() - 1 &&
                j != patch_coords[i].size() - 1) {
                di.push_back(1);
                dj.push_back(1);
            }
            if (i != patch_coords.size() - 1) {
                di.push_back(1);
                dj.push_back(0);
            }

            vector<V3> norms;
            vector<attr_point> attrs;

            for (int k = 0; k < (int)di.size(); ++k) {
                attr_point a;
                V3 p = patch_coords[i + di[k]][j + dj[k]];
                a.coord[0] = p.x;
                a.coord[1] = p.y;
                a.coord[2] = p.z;
                a.coord[3] = 1.0;
                a.coord[4] = 1.0;

                if (render_m_attr.geom_flag) {
                    a.coord[render_m_attr.geometry] = p.x;
                    a.coord[render_m_attr.geometry + 1] = p.y;
                    a.coord[render_m_attr.geometry + 2] = p.z;
                }

                if (render_m_attr.normal_flag) {
                    V3 norm = patch_normals[i + di[k]][j + dj[k]];
                    norms.push_back(norm);
                    a.coord[render_m_attr.normal] = norm.x;
                    a.coord[render_m_attr.normal + 1] = norm.y;
                    a.coord[render_m_attr.normal + 2] = norm.z;
                }
                if (render_m_attr.color_flag) {
                    V3 col = patch_colors[i + di[k]][j + dj[k]];
                    a.coord[render_m_attr.color] = col.x;
                    a.coord[render_m_attr.color + 1] = col.y;
                    a.coord[render_m_attr.color + 2] = col.z;
                }

                if (render_m_attr.weight_flag) {
                    a.coord[render_m_attr.weight] =
                        patch_wps[i + di[k]][j + dj[k]].x;
                }

                if (render_m_attr.opacity_flag) {
                    a.coord[render_m_attr.opacity] =
                        patch_wps[i + di[k]][j + dj[k]].y;
                }

                if (render_m_attr.texture_flag) {
                    V3 texture = patch_textures[i + di[k]][j + dj[k]];
                    a.coord[render_m_attr.texture] = texture.x;
                    a.coord[render_m_attr.texture + 1] = texture.y;
                }
                attrs.push_back(a);
            }

            // take the avg of all 4 norms of a poly and use that as polynorm
            if (render_m_attr.normal_flag) {
                V3 avg = avg_normal_vectors(norms);

                poly_normal[0] = avg.x;
                poly_normal[1] = avg.y;
                poly_normal[2] = avg.z;
            }

            for (int k = 0; k < attrs.size() - 1; ++k) {
                poly_pipeline(attrs[k], MOVE);
            }
            poly_pipeline(attrs[attrs.size() - 1], DRAW);
        }
    }

    return 0;
}
}  // namespace render_direct
// will be implemented later
int REDirect::rd_sqsphere(float radius, float north, float east, float zmin,
                          float zmax, float thetamax) {
    return 0;
}

int REDirect::rd_sqtorus(float radius1, float radius2, float north, float east,
                         float phimin, float phimax, float thetamax) {
    return 0;
}
