#version 460 core
layout(local_size_x = 3, local_size_y = 3, local_size_z = 3) in;

layout(std430, binding=3) readonly buffer particles_buffer {
    vec4 particles[];
};

layout(std430, binding=7) writeonly buffer field_data_buffer {
    float field_data[];
};

uniform int particles_count;
uniform float field_size;
uniform int field_rows;
uniform int field_cols;
uniform int field_planes;
uniform float density_radius;
uniform float particle_mass;
uniform vec3 min_bound;
uniform vec3 max_bound;

float M_PI = 3.14159265358979323846;

// cubic smoothing function
float smoothing(float radius, float diff) {
    if (diff < radius) {
        float volume = (64. * M_PI * pow(radius, 9.)) / 315.0;
        return pow(radius * radius - diff * diff, 3.) / volume;
    }
    return 0.;
}

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
float calc_density(vec3 particle) {
    double density = 0;
    for (int i = 0; i < particles_count; ++i) {
        float d = length(particles[i].xyz - particle);
        density += particle_mass * smoothing(density_radius, d);
    }
    return float(density);
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint z = gl_GlobalInvocationID.z;

    if (x >= field_rows || y >= field_cols || z >= field_planes) return;

    int i = int(x) * field_cols * field_planes;
    int j = int(y) * field_planes;
    int k = int(z);

    int index = i + j + k;

    vec3 field_particle = min_bound + vec3(float(x) * field_size + field_size * .5, float(y) * field_size + field_size * .5, float(z) * field_size + field_size * .5);

    field_data[index] = calc_density(field_particle);
}