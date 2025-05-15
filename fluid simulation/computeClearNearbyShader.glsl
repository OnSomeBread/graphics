#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

struct Entry {
    int particle_idx;
    int grid_idx;
};

layout(std430, binding=7) buffer nearby_buffer {
    Entry nearby[];
};

uniform int particles_count;

#define INT_MAX 2147483647

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    nearby[i] = Entry(INT_MAX, INT_MAX);
}