#include "proj3.h"

// #include <iostream>

// using std::cout;
// using std::endl;

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
VN eval_bezier_curve(vector<VN> &coords, float t) {
    VN ans;
    int n = coords.size() - 1;
    for (int k = 0; k <= n; ++k) {
        ans = ans + (coords[k] * bernstein(n, k, t));
    }
    return ans;
}

// bernstein calculation of bezier patch for particular u and v
// uses all control points
VN eval_bezier_patch(vector<vector<VN>> &coords, float u, float v) {
    VN ans;
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

// used by both bezier curves and patches to grab xyz coords from user
vector<VN> get_points(const vector<float> &vertex) {
    if (data_m_attr.geom_flag) {
        render_m_attr.add_geometry();
    }
    if (data_m_attr.color_flag) {
        render_m_attr.add_color();
    }
    if (data_m_attr.texture_flag) {
        render_m_attr.add_texture();
    }
    if (data_m_attr.weight_flag) {
        render_m_attr.add_weight();
    }
    if (data_m_attr.opacity_flag) {
        render_m_attr.add_opacity();
    }
    vector<VN> points;
    for (int i = 0; i < vertex.size(); i += data_m_attr.size) {
        VN point;
        if (data_m_attr.geom_flag) {
            point.attrs.push_back(vertex[i + data_m_attr.geometry]);
            point.attrs.push_back(vertex[i + data_m_attr.geometry + 1]);
            point.attrs.push_back(vertex[i + data_m_attr.geometry + 2]);
        }
        if (data_m_attr.color_flag) {
            point.attrs.push_back(vertex[i + data_m_attr.color]);
            point.attrs.push_back(vertex[i + data_m_attr.color + 1]);
            point.attrs.push_back(vertex[i + data_m_attr.color + 2]);
        }
        if (data_m_attr.texture_flag) {
            point.attrs.push_back(vertex[i + data_m_attr.texture]);
            point.attrs.push_back(vertex[i + data_m_attr.texture + 1]);
        }
        if (data_m_attr.weight_flag) {
            point.attrs.push_back(vertex[i + data_m_attr.weight]);
        }
        if (data_m_attr.opacity_flag) {
            point.attrs.push_back(vertex[i + data_m_attr.opacity]);
        }

        points.push_back(point);
    }
    return points;
}

attr_point create_attr(VN point) {
    attr_point a;
    a.coord[0] = point.attrs[0];
    a.coord[1] = point.attrs[1];
    a.coord[2] = point.attrs[2];
    a.coord[3] = 1.0;
    a.coord[4] = 1.0;

    int i = -1;
    if (render_m_attr.geom_flag) {
        a.coord[render_m_attr.geometry] = point.attrs[++i];
        a.coord[render_m_attr.geometry + 1] = point.attrs[++i];
        a.coord[render_m_attr.geometry + 2] = point.attrs[++i];
    }

    if (render_m_attr.color_flag) {
        a.coord[render_m_attr.color] = point.attrs[++i];
        a.coord[render_m_attr.color + 1] = point.attrs[++i];
        a.coord[render_m_attr.color + 2] = point.attrs[++i];
    }

    if (render_m_attr.texture_flag) {
        a.coord[render_m_attr.texture] = point.attrs[++i];
        a.coord[render_m_attr.texture + 1] = point.attrs[++i];
    }

    if (render_m_attr.weight_flag) {
        a.coord[render_m_attr.weight] = point.attrs[++i];
    }

    if (render_m_attr.opacity_flag) {
        a.coord[render_m_attr.opacity] = point.attrs[++i];
    }

    return a;
}

// given any number of normal vectors returns the average of them all
V3 avg_normal_vectors(vector<V3> &norms) {
    V3 avg;
    for (int i = 0; i < norms.size(); ++i) {
        avg = avg + norms[i];
    }
    return normalize(avg / -norms.size());
}

namespace render_direct {

int render_bezier_curve(const string &vertex_type, int degree,
                        const vector<float> &vertex) {
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    vector<VN> points = get_points(vertex);
    if (points.size() == 0) {
        return 1;
    }

    render_m_attr.add_shading_offset();

    vector<VN> bezier_points;
    for (int i = 0; i < n_divisions + 1; ++i) {
        float t = i / (float)n_divisions;
        bezier_points.push_back(eval_bezier_curve(points, t));
    }

    attr_point start = create_attr(bezier_points[0]);
    line_pipeline(start, MOVE);

    for (int i = 1; i < bezier_points.size(); ++i) {
        attr_point s = create_attr(bezier_points[i]);
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

    vector<VN> pre_points = get_points(vertex);
    if (pre_points.size() == 0) {
        return 0;
    }

    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    vector<vector<VN>> points;
    vector<vector<V3>> coords;
    // put the coords into v_degree + 1 by u_degree + 1 vector for calculating
    for (int i = 0; i < v_degree + 1; ++i) {
        vector<VN> pr;
        vector<V3> coord;
        for (int j = 0; j < u_degree + 1; ++j) {
            pr.push_back(pre_points[i * (u_degree + 1) + j]);
            coord.push_back(pre_points[i * (u_degree + 1) + j].get_coord());
        }
        points.push_back(pr);
        coords.push_back(coord);
    }

    vector<vector<VN>> patch_coords;
    vector<vector<V3>> patch_normals;

    for (int i = 0; i < n_divisions + 1; ++i) {
        float u = i / (float)n_divisions;
        vector<VN> ncr;
        vector<V3> ncr_normals;
        for (int j = 0; j < n_divisions + 1; ++j) {
            float v = j / (float)n_divisions;
            ncr.push_back(eval_bezier_patch(points, u, v));
            // calculate the normal at same point
            // normalize(cross_product(partialdu, partialdv))
            if (render_m_attr.normal_flag) {
                ncr_normals.push_back(
                    normalize(cross_product(du_bezier_patch(coords, u, v),
                                            dv_bezier_patch(coords, u, v))));
            }
        }

        patch_coords.push_back(ncr);
        patch_normals.push_back(ncr_normals);
    }

    // draw the bezier patch
    for (int i = 0; i < patch_coords.size(); ++i) {
        for (int j = 0; j < patch_coords[i].size(); ++j) {
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

            for (int k = 0; k < di.size(); ++k) {
                attr_point a = create_attr(patch_coords[i + di[k]][j + dj[k]]);

                if (render_m_attr.normal_flag) {
                    V3 norm = patch_normals[i + di[k]][j + dj[k]];
                    norms.push_back(norm);
                    a.coord[render_m_attr.normal] = norm.x;
                    a.coord[render_m_attr.normal + 1] = norm.y;
                    a.coord[render_m_attr.normal + 2] = norm.z;
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
