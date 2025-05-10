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

// layout(std430, binding=6){
//     vec3 nearby[];
// }

uniform int particles_count;
uniform float density_radius;
uniform float particle_mass;
uniform float gravity;
uniform float dt;

float M_PI = 3.14159265358979323846;

// cubic smoothing function
float smoothing(float radius, float diff) {
    if (diff < radius) {
        float volume = (64. * M_PI * pow(radius, 9.)) / 315.0;
        return pow(radius * radius - diff * diff, 3.) / volume;
    }
    return 0;
}

// derivative of the smoothing function
float dsmoothing(float radius, float diff) {
    if (diff < radius) {
        float volume = (64. * M_PI * pow(radius, 9.)) / 315.0;
        return -6. * pow(radius * radius - diff * diff, 2.) * diff / volume;
    }
    return 0;
}

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
float calc_density(vec3 particle) {
    double density = 0;
    for (int i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; ++i) {
        float d = length(particles[i].xyz - particle);
        density += particle_mass * smoothing(density_radius, d);
    }
    return float(density);
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    predicted_particles[i].w = calc_density(predicted_particles[i].xyz);
}