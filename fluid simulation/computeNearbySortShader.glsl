#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

struct Entry {
    int particle_idx;
    int grid_idx;
};

layout(std430, binding=7) buffer nearby_buffer {
    Entry nearby[];
};

uniform int k; 
uniform int j; 

void main() {
    uint i = gl_GlobalInvocationID.x;
    uint ixj = i ^ j;

    if (ixj > i) {
        bool ascending = ((i & k) == 0);

        Entry tempA = nearby[i];
        Entry tempB = nearby[ixj];

        bool swap = (ascending && (tempA.grid_idx > tempB.grid_idx)) || (!ascending && (tempA.grid_idx < tempB.grid_idx));
        if (swap) {
            nearby[i] = tempB;
            nearby[ixj] = tempA;
        }
    }
}
