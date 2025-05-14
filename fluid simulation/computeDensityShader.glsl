#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=5) buffer predicted_particles_buffer {
    vec4 predicted_particles[];
};

layout(std430, binding=7) readonly buffer nearby_buffer {
    int nearby[];
};

layout(std430, binding=8) readonly buffer nearby_counts_buffer {
    int nearby_counts[];
};

uniform int particles_count;
uniform int bucket_rows;
uniform int bucket_cols;
uniform int bucket_planes;
uniform int per_bucket;
uniform int buckets;
uniform vec3 min_bound;
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

int compute_bucket_idx(ivec3 p) {
    return p.x * bucket_cols * bucket_planes + p.y * bucket_planes + p.z;
}

float density_from_nearest_particles(vec3 particle) {
    double density = 0.;
    ivec3 base = ivec3(floor(particle - min_bound) / density_radius);

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                ivec3 neighbor = base + ivec3(i,j,k);
                if (any(lessThan(neighbor, ivec3(0))) || any(greaterThanEqual(neighbor, ivec3(bucket_rows, bucket_cols, bucket_planes)))) continue;

                int idx = compute_bucket_idx(neighbor);
                for(int l = 0; l < nearby_counts[idx] && l < per_bucket; ++l) {
                    float d = length(predicted_particles[nearby[idx * per_bucket + l]].xyz - particle);
                    density += particle_mass * smoothing(density_radius, d);
                }
            }
        }
    }

    return float(density);
}

float calc_density(vec3 particle) {
    double density = 0;
    for (int i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; ++i) {
        float d = length(predicted_particles[i].xyz - particle);
        density += particle_mass * smoothing(density_radius, d);
    }
    return float(density);
}

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    predicted_particles[i].w = calc_density(predicted_particles[i].xyz);
}