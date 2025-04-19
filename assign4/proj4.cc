#include "proj4.h"

#include "cs630.h"

using std::abs;
using std::cos;
using std::isinf;
using std::isnan;
using std::pow;
using std::signbit;
using std::sin;

using namespace render_direct;

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

float magnitude(V3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

V3 normalize(V3 v) {
    float mag = magnitude(v);
    if (mag == 0) {
        return v;
    }
    return v / mag;
}

float sc(float val) { return signbit(val) ? -1.0 : 1.0; }

// given any number of normal vectors returns the average of them all
V3 avg_normal_vectors(vector<V3> &norms) {
    V3 avg;
    for (int i = 0; i < norms.size(); ++i) {
        avg = avg + norms[i];
    }
    return normalize(avg / norms.size());
}

void draw_polygons(vector<vector<V3>> &coords, vector<vector<V3>> &normals) {
    for (int i = 0; i < coords.size(); ++i) {
        for (int j = 0; j < coords[i].size(); ++j) {
            vector<int> di = {0};
            vector<int> dj = {0};
            if (j != coords[i].size() - 1) {
                di.push_back(0);
                dj.push_back(1);
            }
            if (i != coords.size() - 1 && j != coords[i].size() - 1) {
                di.push_back(1);
                dj.push_back(1);
            }
            if (i != coords.size() - 1) {
                di.push_back(1);
                dj.push_back(0);
            }

            vector<V3> norms;
            vector<V3> pnts;
            vector<attr_point> attrs;
            for (int k = 0; k < di.size(); ++k) {
                attr_point a;
                V3 p = coords[i + di[k]][j + dj[k]];
                pnts.push_back(p);
                a.coord[0] = p.x;
                a.coord[1] = p.y;
                a.coord[2] = p.z;
                a.coord[3] = 1.0;

                if (render_m_attr.geom_flag) {
                    a.coord[render_m_attr.geometry] = p.x;
                    a.coord[render_m_attr.geometry + 1] = p.y;
                    a.coord[render_m_attr.geometry + 2] = p.z;
                }
                // if (render_m_attr.normal_flag) {
                //     V3 norm = normals[i + di[k]][j + dj[k]];
                //     a.coord[render_m_attr.normal] = norm.x;
                //     a.coord[render_m_attr.normal + 1] = norm.y;
                //     a.coord[render_m_attr.normal + 2] = norm.z;
                //     norms.push_back(norm);
                // }

                norms.push_back(normals[i + di[k]][j + dj[k]]);
                attrs.push_back(a);
            }

            if (render_m_attr.normal_flag && pnts.size() == 4) {
                // V3 avg = avg_normal_vectors(norms);
                // poly_normal[0] = avg.x;
                // poly_normal[1] = avg.y;
                // poly_normal[2] = avg.z;
                V3 norm = cross_product(pnts[2] - pnts[0], pnts[1] - pnts[3]);
                poly_normal[0] = norm.x;
                poly_normal[1] = norm.y;
                poly_normal[2] = norm.z;
            }

            for (int k = 0; k < attrs.size() - 1; ++k) {
                poly_pipeline(attrs[k], MOVE);
            }
            poly_pipeline(attrs[attrs.size() - 1], DRAW);
        }
    }
}

// SPHERE
int REDirect::rd_sqsphere(float radius, float north, float east, float zmin,
                          float zmax, float thetamax) {
    obj_normal_flag = true;
    render_m_attr.clear();
    render_m_attr.add_geometry();
    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    vector<vector<V3>> coords;
    vector<vector<V3>> normals;

    for (int i = 0; i < n_divisions + 1; ++i) {
        float u = (2.0 * M_PI * i) / (float)n_divisions;

        vector<V3> rows;
        vector<V3> rows_normals;
        for (int j = 0; j < n_divisions + 1; ++j) {
            float v = (M_PI * j) / (float)n_divisions - (M_PI / 2.0);
            V3 p;

            // x = cos^n(v)cos^e(u)
            float xleft = pow(abs(cos(v)), north) * sc(cos(v));
            float xright = pow(abs(cos(u)), east) * sc(cos(u));
            p.x = xleft * xright;

            // y = cos^n(v)sin^e(u)
            float yleft = xleft;
            float yright = pow(abs(sin(u)), east) * sc(sin(u));
            p.y = yleft * yright;

            // z = sin^n(v)
            p.z = pow(abs(sin(v)), north) * sc(sin(v));

            rows.push_back(p);

            // haha normal time
            // calculate the gradient for sqsphere
            // partial derivative with respect to u
            // V3 du_normal;

            // float dxright =
            //     east * pow(abs(cos(u)), east - 1) * sc(cos(u)) * -sin(u);
            // dxright = isnan(dxright) || isinf(dxright) ? -sin(u) : dxright;

            // float dyright =
            //     east * pow(abs(sin(u)), east - 1) * sc(sin(u)) * cos(u);
            // dyright = isnan(dyright) || isinf(dyright) ? cos(u) : dyright;

            // if (east == 0) {
            //     dxright = 1;
            //     dyright = 1;
            // }

            // // x = cos^n(v)ecos^(e-1)(u)(-sin(u))
            // du_normal.x = xleft * dxright;
            // // y = cos^n(v)esin^(e-1)(u)cos(u)
            // du_normal.y = yleft * dyright;
            // // z = 0
            // du_normal.z = 0;

            // // partial derivative with respect to v
            // V3 dv_normal;

            // float dxleft =
            //     north * pow(abs(cos(v)), north - 1) * sc(cos(v)) * -sin(v);
            // dxleft = isnan(dxleft) || isinf(dxleft) ? -sin(v) : dxleft;
            // if (north == 0) {
            //     dxleft = 1;
            // }

            // float dyleft = dxleft;

            // // x = ncos^(n-1)(v)(-sin(v))cos^e(u)
            // dv_normal.x = dxleft * xright;
            // // y = ncos^(n-1)(v)(-sin(v))cos^e(u)
            // dv_normal.y = dyleft * yright;
            // // z = nsin^(n-1)(v)cos(v)
            // dv_normal.z =
            //     north * pow(abs(sin(v)), north - 1) * sc(sin(v)) * cos(v);
            // dv_normal.z =
            //     isnan(dv_normal.z) || isinf(dv_normal.z) ? cos(v) :
            //     dv_normal.z;

            // if (north == 0) {
            //     dv_normal.z = 1;
            // }

            // V3 normal = normalize(cross_product(dv_normal, du_normal));

            // if (magnitude(normal) == 0) {
            //     cout << "u:" << u << " v:" << v << endl;
            //     du_normal.p();
            //     dv_normal.p();
            //     normal.p();
            // }

            // different approach
            // f(x, y, z) = ( |x|^(2/e) + |y|^(2/e) )^(e/n) + |z|^(2/n) - 1
            V3 norm;
            norm.x = east / north *
                     pow(pow(abs(p.x), 2.0 / east) + pow(abs(p.y), 2.0 / east),
                         east / north - 1) *
                     2.0 / east * sc(p.x) * pow(abs(p.x), 2.0 / east - 1);

            norm.y = east / north *
                     pow(pow(abs(p.x), 2.0 / east) + pow(abs(p.y), 2.0 / east),
                         east / north - 1) *
                     2.0 / east * sc(p.y) * pow(abs(p.y), 2.0 / east - 1);

            norm.z = (2.0 / north) * pow(abs(p.z), 2.0 / north - 1) * sc(p.z);

            norm.x = isnan(norm.x) || isinf(norm.x) ? sc(p.x) : norm.x;
            norm.y = isnan(norm.y) || isinf(norm.y) ? sc(p.y) : norm.y;
            norm.z = isnan(norm.z) || isinf(norm.z) ? sc(p.z) : norm.z;

            rows_normals.push_back(normalize(norm));
        }
        coords.push_back(rows);
        normals.push_back(rows_normals);
    }

    draw_polygons(coords, normals);

    return 0;
}

// TORUS
int REDirect::rd_sqtorus(float radius1, float radius2, float north, float east,
                         float phimin, float phimax, float thetamax) {
    obj_normal_flag = true;
    render_m_attr.clear();
    render_m_attr.add_geometry();
    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    vector<vector<V3>> coords;
    vector<vector<V3>> normals;

    for (int i = 0; i < n_divisions + 1; ++i) {
        float u = (2.0 * M_PI * i) / (float)n_divisions;

        vector<V3> rows;
        vector<V3> rows_normals;
        for (int j = 0; j < n_divisions + 1; ++j) {
            float v = (2.0 * M_PI * j) / (float)n_divisions - M_PI;
            V3 p;

            // x = cos(u)(R + r cos(v))
            float xleft = pow(abs(cos(u)), east) * sc(cos(u));
            float xright =
                radius1 + radius2 * pow(abs(cos(v)), north) * sc(cos(v));
            p.x = xleft * xright;

            // y = sin(u)(R + r cos(v))
            float yleft = pow(abs(sin(u)), east) * sc(sin(u));
            float yright =
                radius1 + radius2 * pow(abs(cos(v)), north) * sc(cos(v));
            p.y = yleft * yright;

            // z = r sin(v)
            p.z = radius2 * pow(abs(sin(v)), north) * sc(sin(v));

            rows.push_back(p);

            // sqtorus normals
            V3 du_normal;
            float dxleft =
                east * pow(abs(cos(u)), east - 1) * sc(cos(u)) * (-sin(u));
            // dxleft = isnan(dxleft) || isinf(dxleft) ? -sin(u) : dxleft;

            float dyleft =
                east * pow(abs(sin(u)), east - 1) * sc(sin(u)) * cos(u);
            // dyleft = isnan(dyleft) || isinf(dyleft) ? cos(u) : dyleft;

            if (east == 0) {
                dxleft = 1;
                dyleft = 1;
            }
            du_normal.x = dxleft * xright;
            du_normal.y = dyleft * yright;
            du_normal.z = 0;

            V3 dv_normal;

            float dxright = radius2 * north * pow(abs(cos(v)), north - 1) *
                            sc(cos(v)) * (-sin(v));
            // dxright =
            //     isnan(dxright) || isinf(dxright) ? -sin(v) * radius2 :
            //     dxright;

            if (north == 0) {
                dxright = 1;
            }
            float dyright = dxright;

            dv_normal.x = xleft * dxright;
            dv_normal.y = yleft * dyright;

            dv_normal.z = radius2 * north * pow(abs(sin(v)), north - 1) *
                          sc(sin(v)) * cos(v);
            // dv_normal.z = isnan(dv_normal.z) || isinf(dv_normal.z)
            //                   ? cos(v) * radius2
            //                   : dv_normal.z;
            if (north == 0) {
                dv_normal.z = 1;
            }

            V3 normal = normalize(cross_product(du_normal, dv_normal));

            // just a copy of sqsphere
            // f(x, y, z) = (|sqrt(x*x + y*y)-R|^(2/e) + |z|^(2/n))^(e*n/2) - r
            V3 norm;
            norm.x = east / north *
                     pow(pow(abs(p.x), 2.0 / east) + pow(abs(p.y), 2.0 / east),
                         east / north - 1) *
                     2.0 / east * sc(p.x) * pow(abs(p.x), 2.0 / east - 1);

            norm.y = east / north *
                     pow(pow(abs(p.x), 2.0 / east) + pow(abs(p.y), 2.0 / east),
                         east / north - 1) *
                     2.0 / east * sc(p.y) * pow(abs(p.y), 2.0 / east - 1);

            norm.z = (2.0 / north) * pow(abs(p.z), 2.0 / north - 1) * sc(p.z);

            norm.x = isnan(norm.x) || isinf(norm.x) ? sc(p.x) : norm.x;
            norm.y = isnan(norm.y) || isinf(norm.y) ? sc(p.y) : norm.y;
            norm.z = isnan(norm.z) || isinf(norm.z) ? sc(p.z) : norm.z;

            // rows_normals.push_back(normalize(norm));
            rows_normals.push_back(norm);
        }
        coords.push_back(rows);
        normals.push_back(rows_normals);
    }

    draw_polygons(coords, normals);

    return 0;
}
namespace render_direct {
// these 2 functions have already been implemented in bezier curves and surfaces
int render_bezier_curve(const string &vertex_type, int degree,
                        const vector<float> &vertex) {
    return 1;
}

int render_bezier_patch(const string &vertex_type, int u_degree, int v_degree,
                        const vector<float> &vertex) {
    return 1;
}

}  // namespace render_direct