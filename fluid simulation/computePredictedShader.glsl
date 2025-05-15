#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=3) readonly buffer particles_buffer {
    vec4 particles[];
};

layout(std430, binding=4) buffer velocities_buffer {
    vec4 velocities[];
};

layout(std430, binding=5) buffer predicted_particles_buffer {
    vec4 predicted_particles[];
};

struct Entry {
    int particle_idx;
    int grid_idx;
};

layout(std430, binding=7) writeonly buffer nearby_buffer {
    Entry nearby[];
};

uniform int particles_count;
uniform float density_radius;
uniform float gravity;
uniform float dt;

// massive optimization to allow for more particles
// split the bounding box into grid squares of size density_radius
// then when calculating densities or pressure forces only consider the
// 3x3 grid of cells since the rest of the particles will be 0 anyway
// the spatial hash
int hash_function(ivec3 p) {
    return ((p.x * 73856093) + (p.y * 19349663) + (p.z * 83492791)) % particles_count;
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    // add gravitational forces to particles
    velocities[i].z -= gravity * dt;
    
    //predicted_particles[i] = particles[i] + velocities[i] * dt;
    // use a constant look ahead factor since for whatever reason it makes
    // particles less jiggly at higher fps
    predicted_particles[i] = particles[i] + velocities[i] * (1./120.);

    vec3 p = predicted_particles[i].xyz / density_radius;
    nearby[i] = Entry(int(i), hash_function(ivec3(p.x, p.y, p.z)));
}