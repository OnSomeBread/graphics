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
uniform float target_density;
uniform float density_radius;
uniform float particle_mass;
uniform float pressure_multiplier;
uniform float viscosity_multiplier;
uniform float dt;

uniform float particle_damping;
uniform vec3 min_bound;
uniform vec3 max_bound;

float M_PI = 3.14159265358979323846;

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


// used for calculating near pressure forces
float steeperSmoothing(float dst, float radius) {
    if (dst < radius) {
        float volume = (M_PI * pow(radius, 6.)) / 15.0;
        return pow(radius - dst, 3.) * volume;
    }
    return 0.;
}

float DsteeperSmoothing(float dst, float radius) {
    if (dst <= radius) {
        float volume = (pow(radius, 6.) * M_PI) / 35.0;
        return -pow(radius - dst, 2.) / volume;
    }
    return 0.;
}

// calculate the pressure force for the particle
vec3 pressure_force(vec3 currParticle, float currDensity, uint idx, float target_density, float radius,
                  float mass) {
    dvec3 pressure = dvec3(0.,0.,0.);

    for (int i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; ++i) {
        vec3 particle_diff = predicted_particles[i].xyz - currParticle;
        if(particle_diff.x == 0 && particle_diff.y == 0 && particle_diff.z == 0){
            continue;
        }

        float d = length(particle_diff);
        float ds = dsmoothing(radius, d);

        // if moving in same direction pick a random one instead
        vec3 dir = d == 0 ? random_dir(float(idx + i)) : particle_diff / d;

        // instead of just p0 use the average particle pressure between the
        // current particle and this particle
        float p0 = (target_density - predicted_particles[i].w);
        float p1 = (target_density - currDensity);
        float avgp = (p0 + p1) / 2.0;

        pressure += dir * avgp * mass * ds;
    }
    return vec3(pressure);
}

vec3 viscosity_force(vec3 currParticle, vec3 currVelocity, float radius) {
    dvec3 viscosity = dvec3(0.,0.,0.);
    for (int i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; ++i) {
        vec3 particle_diff = currParticle - predicted_particles[i].xyz;
        if(particle_diff.x == 0 && particle_diff.y == 0 && particle_diff.z == 0){
            continue;
        }
        float d = length(particle_diff);
        float s = smoothing(radius, d);
        viscosity += (velocities[i].xyz - currVelocity) * s;
    }

    return vec3(viscosity);
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    // add particle pressure forces
    // tells the particle how fast it should conform to target density
    vec3 pressure_accel = pressure_force(predicted_particles[i].xyz, predicted_particles[i].w, i, target_density, density_radius, particle_mass) * pressure_multiplier / predicted_particles[i].w;
    velocities[i] += vec4(pressure_accel * dt, 0.);

    // add viscosity force
    // creates friction between nearby particles so that particles within
    // the radius have similar velocities
    vec3 viscosity_accel = viscosity_force(predicted_particles[i].xyz, velocities[i].xyz, density_radius);
    velocities[i] += vec4(viscosity_accel * viscosity_multiplier * dt, 0.);

    particles[i] += velocities[i] * dt;

    if (particles[i].x < min_bound.x) {
        particles[i].x = min_bound.x;
        velocities[i].x *= -particle_damping;
    }
    if (particles[i].x > max_bound.x) {
        particles[i].x = max_bound.x;
        velocities[i].x *= -particle_damping;
    }

    if (particles[i].y < min_bound.y) {
        particles[i].y = min_bound.y;
        velocities[i].y *= -particle_damping;
    }
    if (particles[i].y > max_bound.y) {
        particles[i].y = max_bound.y;
        velocities[i].y *= -particle_damping;
    }

    if (particles[i].z < min_bound.z) {
        particles[i].z = min_bound.z;
        velocities[i].z *= -particle_damping;
    }
    if (particles[i].z > max_bound.z) {
        particles[i].z = max_bound.z;
        velocities[i].z *= -particle_damping;
    }
}