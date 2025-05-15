#version 460 core
layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(std430, binding=3) readonly buffer particles_buffer {
    vec4 particles[];
};

layout(std430, binding=6) writeonly buffer field_data_buffer {
    float field_data[];
};

struct Entry {
    int particle_idx;
    int grid_idx;
};

layout(std430, binding=7) readonly buffer nearby_buffer {
    Entry nearby[];
};

layout(std430, binding=8) readonly buffer nearby_idx_buffer {
    int nearby_idx[];
};

#define INT_MAX 2147483647

uniform int particles_count;
uniform float field_size;
uniform int field_rows;
uniform int field_cols;
uniform int field_planes;
uniform float density_radius;
uniform float particle_mass;
uniform vec3 min_bound;

float M_PI = 3.14159265358979323846;

// cubic smoothing function
float smoothing(float radius, float diff) {
    if (diff < radius) {
        float volume = (64. * M_PI * pow(radius, 9.)) / 315.0;
        return pow(radius * radius - diff * diff, 3.) / volume;
    }
    return 0;
}

// the spatial hash
int hash_function(ivec3 p) {
    return ((p.x * 73856093) + (p.y * 19349663) + (p.z * 83492791)) % particles_count;
}

float calc_density(vec3 particle) {
    double density = 0.;
    ivec3 base = ivec3(particle / density_radius);

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int idx = nearby_idx[hash_function(base + ivec3(i,j,k))];

                // this means that no such particle exists at those coordinates
                if(idx == INT_MAX) continue;
                int value = nearby[idx].grid_idx;

                for(int l = 0; nearby[idx + l].grid_idx == value && l < particles_count; ++l) {
                    float d = length(particles[nearby[idx + l].particle_idx].xyz - particle);
                    density += smoothing(density_radius, d);
                }
            }
        }
    }

    return float(density) * particle_mass;
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