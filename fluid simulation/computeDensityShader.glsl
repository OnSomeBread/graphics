#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=5) buffer predicted_particles_buffer {
    vec4 predicted_particles[];
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
uniform vec3 bound_size;
uniform float density_radius;
uniform float particle_mass;

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
                    float d = length(predicted_particles[nearby[idx + l].particle_idx].xyz - particle);
                    density += smoothing(density_radius, d);
                }
            }
        }
    }

    return float(density) * particle_mass;
}

// brute force density calculation for reference
// float calc_density(vec3 particle) {
//     double density = 0;
//     for (int i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; ++i) {
//         float d = length(predicted_particles[i].xyz - particle);
//         density += smoothing(density_radius, d);
//     }
//     return float(density) * particle_mass;
// }

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    predicted_particles[i].w = calc_density(predicted_particles[i].xyz);
}