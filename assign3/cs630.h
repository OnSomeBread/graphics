#ifndef CS_630_H
#define CS_630_H

#include <string>
using std::string;
#include <vector>
using std::vector;

class RenderEngine {
   public:
    /********************** General functions *******************************/

    virtual int rd_display(const string &name, const string &type,
                           const string &mode);

    virtual int rd_format(int xresolution, int yresolution);

    virtual int rd_world_begin(void);
    virtual int rd_world_end(void);

    virtual int rd_frame_begin(int frame_no);
    virtual int rd_frame_end(void);

    virtual int rd_render_init(void); /* Initialize renderer */
    virtual int rd_render_cleanup(void);

    /********************** Camera ******************************************/

    virtual int rd_camera_eye(const float eyepoint[3]);
    virtual int rd_camera_at(const float atpoint[3]);
    virtual int rd_camera_up(const float up[3]);
    virtual int rd_camera_fov(float fov);
    virtual int rd_clipping(float znear, float zfar);

    /********************** Transformations ***********************************/

    virtual int rd_translate(const float offset[3]);
    virtual int rd_scale(const float scale_factor[3]);
    virtual int rd_rotate_xy(float angle);
    virtual int rd_rotate_yz(float angle);
    virtual int rd_rotate_zx(float angle);

    virtual int rd_xform_push(void);
    virtual int rd_xform_pop(void);

    /********************** Geometric Objects *******************************/

    virtual int rd_bezier_curve(const string &vertex_type, int degree,
                                const vector<float> &vertex);

    virtual int rd_bezier_patch(const string &vertex_type, int u_degree,
                                int v_degree, const vector<float> &vertex);

    virtual int rd_catmull_clark_sds(const string &vertex_type,
                                     const vector<float> &coord, int nvertex,
                                     const vector<int> &vertex_list, int nface,
                                     const vector<int> &crease_list,
                                     int ncrease,
                                     const vector<float> &sharpness);

    virtual int rd_circle(const float center[3], float radius);

    virtual int rd_line(const float start[3], const float end[3]);

    virtual int rd_lineset(const string &vertex_type, int nvertex,
                           const vector<float> &vertex, int nseg,
                           const vector<int> &seg);

    virtual int rd_point(const float p[3]);

    virtual int rd_pointset(const string &vertex_type, int nvertex,
                            const vector<float> &vertex);
    virtual int rd_polyset(const string &vertex_type, int nvertex,
                           const vector<float> &vertex, int nface,
                           const vector<int> &face);

    virtual int rd_cone(float height, float radius, float thetamax);
    virtual int rd_cube(void);
    virtual int rd_cylinder(float radius, float zmin, float zmax,
                            float thetamax);
    virtual int rd_disk(float height, float radius, float theta);

    virtual int rd_hyperboloid(const float start[3], const float end[3],
                               float thetamax);

    virtual int rd_paraboloid(float rmax, float zmin, float zmax,
                              float thetamax);
    virtual int rd_sphere(float radius, float zmin, float zmax, float thetamax);
    virtual int rd_sqsphere(float radius, float north, float east, float zmin,
                            float zmax, float thetamax);
    virtual int rd_sqtorus(float radius1, float radius2, float north,
                           float east, float phimin, float phimax,
                           float thetamax);
    virtual int rd_torus(float radius1, float radius2, float phimin,
                         float phimax, float thetamax);
    virtual int rd_tube(const float start[3], const float end[3], float radius);

    /********************** Lighting & Shading ***************************/

    virtual int rd_background(const float color[]);
    // red, green, blue by default

    virtual int rd_color(const float color[]);

    virtual int rd_opacity(float opacity);

    virtual int rd_emission(const float color[], float intensity);

    virtual int rd_fill(const float seed_point[3]);

    virtual int rd_surface(const string &shader_type);

    virtual int rd_cone_light(const float pos[3], const float at[3],
                              float theta_min, float theta_max,
                              const float color[], float intensity);

    virtual int rd_point_light(const float pos[3], const float color[],
                               float intensity);

    virtual int rd_far_light(const float dir[3], const float color[],
                             float intensity);

    virtual int rd_ambient_light(const float color[], float intensity);

    virtual int rd_specular_color(const float color[], int exponent);

    virtual int rd_k_ambient(float Ka);
    virtual int rd_k_diffuse(float Kd);
    virtual int rd_k_emission(float Ke);
    virtual int rd_k_specular(float Ks);

    /**************************** Mapping ******************************/

    virtual int rd_map_border(const string &map_type, const string &horizontal,
                              const string &vertical);
    virtual int rd_map_bound(const string &map_type, float s_min, float t_min,
                             float s_max, float t_max);
    virtual int rd_map_load(const string &filename, const string &label);
    virtual int rd_map_sample(const string &map_type, const string &intra_level,
                              const string &inter_level);
    virtual int rd_map(const string &map_type, const string &label);

    /**************************** Options **********************************/

    virtual int rd_option_array(const string &name, int n,
                                const vector<float> &values);

    virtual int rd_option_bool(const string &name, bool flag);

    virtual int rd_option_list(const string &name, int n,
                               const string values[]);

    virtual int rd_option_real(const string &name, float value);

    virtual int rd_option_string(const string &name, const string &value);

    virtual int rd_custom(const string &label);

    virtual ~RenderEngine();
};

class REDirect : public RenderEngine {
   public:
    int rd_display(const string &name, const string &type, const string &mode);

    int rd_format(int xresolution, int yresolution);

    int rd_frame_begin(int frame_no);
    int rd_frame_end(void);

    int rd_render_init(void); /* Initialize renderer */
    int rd_render_cleanup(void);

    int rd_point(const float p[3]);
    int rd_line(const float start[3], const float end[3]);
    int rd_translate(const float offset[3]);
    int rd_scale(const float scale_factor[3]);
    int rd_rotate_xy(float angle);
    int rd_rotate_yz(float angle);
    int rd_rotate_zx(float angle);
    int rd_camera_eye(const float eyepoint[3]);
    int rd_camera_at(const float atpoint[3]);
    int rd_camera_up(const float up[3]);
    int rd_camera_fov(const float fov);
    int rd_clipping(float znear, float zfar);
    int rd_world_begin(void);
    int rd_world_end(void);

    int rd_bezier_curve(const string &vertex_type, int degree,
                        const vector<float> &vertex);
    int rd_bezier_patch(const string &vertex_type, int u_degree, int v_degree,
                        const vector<float> &vertex);

    int rd_lineset(const string &vertex_type, int nvertex,
                   const vector<float> &vertex, int nseg,
                   const vector<int> &seg);
    int rd_pointset(const string &vertex_type, int nvertex,
                    const vector<float> &vertex);
    int rd_polyset(const string &vertex_type, int nvertex,
                   const vector<float> &vertex, int nface,
                   const vector<int> &face);
    int rd_cube(void);
    int rd_sphere(float radius, float zmin, float zmax, float thetamax);
    int rd_cone(float height, float radius, float thetamax);
    int rd_disk(float height, float radius, float theta);
    int rd_cylinder(float radius, float zmin, float zmax, float thetamax);
    int rd_tube(const float start[3], const float end[3], float radius);
    int rd_hyperboloid(const float start[3], const float end[3],
                       float thetamax);
    int rd_paraboloid(float rmax, float zmin, float zmax, float thetamax);
    int rd_torus(float radius1, float radius2, float phimin, float phimax,
                 float thetamax);

    int rd_sqsphere(float radius, float north, float east, float zmin,
                    float zmax, float thetamax);
    int rd_sqtorus(float radius1, float radius2, float north, float east,
                   float phimin, float phimax, float thetamax);

    int rd_background(const float color[]);
    int rd_color(const float color[]);

    int rd_circle(const float center[3], float radius);
    int rd_fill(const float seed_point[3]);

    int rd_xform_push(void);
    int rd_xform_pop(void);

#ifndef NO_SHADING
    int rd_surface(const string &shader_type);

    int rd_point_light(const float pos[3], const float color[],
                       float intensity);
    int rd_far_light(const float dir[3], const float color[], float intensity);
    int rd_ambient_light(const float color[], float intensity);

    int rd_cone_light(const float pos[3], const float at[3], float theta_min,
                      float theta_max, const float color[], float intensity);

    int rd_specular_color(const float color[], int exponent);

    int rd_k_ambient(float Ka);
    int rd_k_diffuse(float Kd);
    int rd_k_specular(float Ks);
#endif /* NO_SHADING */

#ifndef NO_TEXTURE
    int rd_map_border(const string &map_type, const string &horizontal,
                      const string &vertical);
    int rd_map_bound(const string &map_type, float s_min, float t_min,
                     float s_max, float t_max);
    int rd_map_load(const string &filename, const string &label);
    int rd_map_sample(const string &map_type, const string &intra_level,
                      const string &inter_level);
    int rd_map(const string &map_type, const string &name);
#endif /* NO_TEXTURE */

    int rd_option_bool(const string &, bool flag);
    int rd_option_real(const string &, float value);

    int rd_custom(const string &label);
};

// Some useful helper functions
int get_vertex_size(const string &vertex_type);
// Returns the number of components in an attributed vertex type

struct attr_point {
    static const int START = 4;
    static const int CONSTANT = 4;
    static const int MAX = 24;

    float coord[MAX];
};

struct meta_attribute {
    bool geom_flag;  // Isn't this always set?
    bool color_flag;
    bool normal_flag;
    bool texture_flag;
    bool weight_flag;
    bool opacity_flag;

    int size;

    int geometry;
    int color;
    int normal;
    int texture;
    int weight;
    int opacity;

    meta_attribute();

    void clear();

    void add_geometry();
    void add_color();
    void add_normal();
    void add_texture();
    void add_weight();
    void add_opacity();

    int set_data_indices(const string &vertex_type);
    // Sets the offsets of the given attributes with respect to the start
    // of a vertex.
    // If an attribute is not used, an offset of -1 is returned for that
    // attribute.
    // Also sets the number of values comprising a vertex, i.e. the vertex size.

    int set_render_indices(const string &vertex_type);
    // Similar to set_data_indices, but offsets given for rendering.
    // Some extra variables may be added for rendering purposes.
    // For example, texturing will add some room for needed ds and dt values.

    int add_shading_offset(void);
    // This is called after all of the used attributes are set, either with
    // set_ or with the add_ methods.
    // It simply adds the offsets needed so that the indices reference the
    // locations inside an attributed point.
};

extern meta_attribute data_m_attr;    // For data vertex information
extern meta_attribute render_m_attr;  // For attributed point info in rendering

namespace render_direct {

extern int n_divisions;
// Number of divisions per primitive

extern bool obj_normal_flag;
// Rendering routines should set this to true if poly_normal is in
// object coordinates, false if it is in world coordinates.

extern float poly_normal[3];

#define MOVE 0
#define DRAW 1

int poly_pipeline(attr_point &p, int end_flag);
// Points coming in here are assumed to be in object coordinates

int line_pipeline(attr_point &p, int draw_flag);
// Takes a point in object coordinates and a MOVE/DRAW flag and
// treats it as the endpoint of a line to be drawn.

int render_bezier_curve(const string &vertex_type, int degree,
                        const vector<float> &vertex);

int render_bezier_patch(const string &vertex_type, int u_degree, int v_degree,
                        const vector<float> &vertex);

} /* namespace render_direct */

#endif /* CS_630_H */