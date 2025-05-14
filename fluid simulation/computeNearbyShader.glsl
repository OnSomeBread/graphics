#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=5) readonly buffer predicted_particles_buffer {
    vec4 predicted_particles[];
};

layout(std430, binding=7) buffer nearby_buffer {
    int nearby[];
};

layout(std430, binding=8) buffer nearby_counts_buffer {
    int nearby_counts[];
};

uniform int particles_count;
uniform int bucket_rows;
uniform int bucket_cols;
uniform int bucket_planes;
uniform int buckets;
uniform int per_bucket;
uniform float density_radius;
uniform vec3 min_bound;

// massive optimization to allow for more particles
// split the bounding box into grid squares of size density_radius
// then when calculating densities or pressure forces only consider the
// 3x3 grid of cells since the rest of the particles will be 0 anyway

// this is index for nearby_counts
int get_bucket_idx(vec3 p) {
    ivec3 idx = ivec3(floor(p - min_bound) / density_radius);
    if (any(lessThan(idx, ivec3(0))) || any(greaterThanEqual(idx, ivec3(bucket_rows, bucket_cols, bucket_planes)))) return -1;

    return idx.x * bucket_cols * bucket_planes + idx.y * bucket_planes + idx.z;
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    int idx = get_bucket_idx(predicted_particles[i].xyz);
    if(idx == -1) return;
    int offset = atomicAdd(nearby_counts[idx], 1);
    if (offset >= per_bucket) return ;

    nearby[idx * per_bucket + offset] = int(i);
}