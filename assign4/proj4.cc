#include "proj4.h"

using std::abs;
using std::cos;
using std::pow;
using std::sin;

using namespace render_direct;

int REDirect::rd_sqsphere(float radius, float north, float east, float zmin,
                          float zmax, float thetamax) {
    obj_normal_flag = true;
    render_m_attr.clear();
    render_m_attr.add_geometry();
    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    vector<vector<V3>> coords;

    for (int i = 0; i < n_divisions + 1; ++i) {
        float u = (2.0 * M_PI * i) / (float)n_divisions;

        vector<V3> rows;
        for (int j = 0; j < n_divisions + 1; ++j) {
            float v = (M_PI * j) / (float)n_divisions - (M_PI / 2.0);
            V3 p;

            p.x = (pow(abs(cos(v)), north) * (cos(v) < 0 ? -1.0 : 1.0)) *
                  (pow(abs(cos(u)), east) * (cos(u) < 0 ? -1.0 : 1.0));

            p.y = (pow(abs(cos(v)), north) * (cos(v) < 0 ? -1.0 : 1.0)) *
                  (pow(abs(sin(u)), east) * (sin(u) < 0 ? -1.0 : 1.0));

            p.z = pow(abs(sin(v)), north) * (sin(v) < 0 ? -1.0 : 1.0);

            rows.push_back(p);
        }
        coords.push_back(rows);
    }

    for (int i = 0; i < coords.size() - 1; ++i) {
        for (int j = 0; j < coords[i].size() - 1; ++j) {
            vector<V3> points = {coords[i][j], coords[i][j + 1],
                                 coords[i + 1][j + 1], coords[i + 1][j]};

            vector<attr_point> attrs;
            for (int k = 0; k < points.size(); ++k) {
                attr_point a;
                a.coord[0] = points[k].x;
                a.coord[1] = points[k].y;
                a.coord[2] = points[k].z;
                a.coord[3] = 1.0;
                a.coord[4] = 1.0;
                attrs.push_back(a);
            }

            for (int k = 0; k < attrs.size() - 1; ++k) {
                poly_pipeline(attrs[k], MOVE);
            }
            poly_pipeline(attrs[attrs.size() - 1], DRAW);
        }
    }

    return 0;
}

int REDirect::rd_sqtorus(float radius1, float radius2, float north, float east,
                         float phimin, float phimax, float thetamax) {
    obj_normal_flag = true;
    render_m_attr.clear();
    render_m_attr.add_geometry();
    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    vector<vector<V3>> coords;

    for (int i = 0; i < n_divisions + 1; ++i) {
        float u = (2.0 * M_PI * i) / (float)n_divisions;

        vector<V3> rows;
        for (int j = 0; j < n_divisions + 1; ++j) {
            float v = (2.0 * M_PI * j) / (float)n_divisions - M_PI;
            V3 p;

            p.x = (pow(abs(cos(u)), east) * (cos(u) < 0 ? -1.0 : 1.0)) *
                  (radius1 + radius2 * pow(abs(cos(v)), north) *
                                 (cos(v) < 0 ? -1.0 : 1.0));

            p.y = (pow(abs(sin(u)), east) * (sin(u) < 0 ? -1.0 : 1.0)) *
                  (radius1 + radius2 * pow(abs(cos(v)), north) *
                                 (cos(v) < 0 ? -1.0 : 1.0));

            p.z = radius2 * pow(abs(sin(v)), north) * (sin(v) < 0 ? -1.0 : 1.0);

            rows.push_back(p);
        }
        coords.push_back(rows);
    }

    for (int i = 0; i < coords.size() - 1; ++i) {
        for (int j = 0; j < coords[i].size() - 1; ++j) {
            vector<V3> points = {coords[i][j], coords[i][j + 1],
                                 coords[i + 1][j + 1], coords[i + 1][j]};

            vector<attr_point> attrs;
            for (int k = 0; k < points.size(); ++k) {
                attr_point a;
                a.coord[0] = points[k].x;
                a.coord[1] = points[k].y;
                a.coord[2] = points[k].z;
                a.coord[3] = 1.0;
                a.coord[4] = 1.0;
                attrs.push_back(a);
            }

            for (int k = 0; k < attrs.size() - 1; ++k) {
                poly_pipeline(attrs[k], MOVE);
            }
            poly_pipeline(attrs[attrs.size() - 1], DRAW);
        }
    }

    return 0;
}
namespace render_direct {
// these 2 functions have already been implemented in assign3
int render_bezier_curve(const string &vertex_type, int degree,
                        const vector<float> &vertex) {
    return 1;
}

int render_bezier_patch(const string &vertex_type, int u_degree, int v_degree,
                        const vector<float> &vertex) {
    return 1;
}

}  // namespace render_direct