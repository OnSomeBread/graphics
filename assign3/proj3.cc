#include "cs630.h"

int RenderEngine::rd_bezier_curve(const string &vertex_type, int degree,
                                  const vector<float> &vertex) {
    // At the beginning of the function
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    render_m_attr.add_shading_offset();

    return 0;
}

int RenderEngine::rd_bezier_patch(const string &vertex_type, int u_degree,
                                  int v_degree, const vector<float> &vertex) {
    // At the beginning of the function
    data_m_attr.clear();
    int err = data_m_attr.set_data_indices(vertex_type);
    if (err) return err;
    err = render_m_attr.set_render_indices(vertex_type);
    if (err) return err;

    render_m_attr.add_normal();
    render_m_attr.add_shading_offset();

    return 0;
}

int rd_sqsphere(float radius, float north, float east, float zmin, float zmax,
                float thetamax) {
    return 0;
}

int rd_sqtorus(float radius1, float radius2, float phimin, float phimax,
               float thetamax) {
    return 0;
}