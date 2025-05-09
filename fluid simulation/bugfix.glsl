#version 460 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
//layout(rgba32f, binding = 0) uniform image2D screen;

layout(std430, binding=3) buffer particles_buffer {
    vec3 particles[];
};

layout(std430, binding=4) buffer velocities_buffer {
    vec3 velocities[];
};

layout(std430, binding=5) buffer predicted_particles_buffer {
    vec3 predicted_particles[];
};

// layout(std430, binding=6){
//     vec3 nearby[];
// }

layout(std430, binding=7) buffer densities_buffer {
    float densities[];
};

uniform int particles_count;
uniform float target_density;
uniform float density_radius;
uniform float particle_mass;
uniform float pressure_multiplier;
uniform float viscosity_multiplier;
uniform float dt;

float M_PI = 3.1415926535897932;

float random(float idx) {
    return fract(sin(dot(idx,12.9898))*43758.5453123);
}

vec3 random_dir(float idx) {
    return vec3(random(idx + 73856093.), random(idx + 19349663.), random(idx + 83492791.));
}

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

// calculate the pressure force for the particle
vec3 pressure_force(vec3 currParticle, float currDensity, uint idx, float target_density, float radius,
                  float mass) {
    dvec3 pressure = dvec3(0.,0.,0.);

    for (int i = 0; i < particles.length(); ++i) {
        vec3 particle_diff = predicted_particles[i] - currParticle;

        float d = length(particle_diff);
        float ds = dsmoothing(radius, d);

        // if moving in same direction pick a random one instead
        vec3 dir = abs(d) < 1e-6 ? random_dir(float(idx + i)) : particle_diff / d;

        // instead of just p0 use the average particle pressure between the
        // current particle and this particle
        float p0 = (target_density - densities[i]);
        float p1 = (target_density - currDensity);
        float avgp = (p0 + p1) / 2.0;

        pressure += dir * avgp * mass * ds;
    }
    return vec3(pressure);
}

vec3 viscosity_force(vec3 currParticle, vec3 currVelocity, float radius) {
    dvec3 viscosity = dvec3(0.,0.,0.);
    for (int i = 0; i < particles.length(); ++i) {
        vec3 particle_diff = currParticle - predicted_particles[i];
        if(particle_diff.x == 0 && particle_diff.y == 0 && particle_diff.z == 0){
            continue;
        }
        float d = length(particle_diff);
        float s = smoothing(radius, d);
        viscosity += (velocities[i] - currVelocity) * s;
    }

    return vec3(viscosity);
}

void main() {
    uint i = gl_GlobalInvocationID.x;

    velocities[0] = vec3(float(densities.length()), float(velocities.length()), -1.0);
    //velocities[0] = vec3(123.0, 456.0, 789.0);
}