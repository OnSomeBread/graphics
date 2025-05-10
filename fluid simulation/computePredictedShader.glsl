#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
//layout(rgba32f, binding = 0) uniform image2D screen;

layout(std430, binding=3) buffer particles_buffer {
    vec4 particles[];
};

layout(std430, binding=4) buffer velocities_buffer {
    vec4 velocities[];
};

layout(std430, binding=5) buffer predicted_particles_buffer {
    vec4 predicted_particles[];
};

uniform int particles_count;
uniform float gravity;
uniform float dt;

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    // add gravitational forces to particles
    velocities[i].z -= gravity * dt;
    predicted_particles[i] = particles[i] + velocities[i] * dt;
}