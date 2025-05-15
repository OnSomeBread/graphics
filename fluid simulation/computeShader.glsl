#version 460 core
layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=3) buffer particles_buffer {
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

layout(std430, binding=7) readonly buffer nearby_buffer {
    Entry nearby[];
};

layout(std430, binding=8) readonly buffer nearby_idx_buffer {
    int nearby_idx[];
};

#define INT_MAX 2147483647

uniform int particles_count;
uniform float target_density;
uniform float density_radius;
uniform float particle_mass;
uniform float pressure_multiplier;
uniform float near_pressure_multiplier = 5;
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

// the spatial hash
int hash_function(ivec3 p) {
    return ((p.x * 73856093) + (p.y * 19349663) + (p.z * 83492791)) % particles_count;
}

vec3 pressure_force(vec4 particle) {
    dvec3 pressure = dvec3(0.,0.,0.);
    ivec3 base = ivec3(particle.xyz / density_radius);

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int idx = nearby_idx[hash_function(base + ivec3(i,j,k))];

                // this means that no such particle exists at those coordinates
                if(idx == INT_MAX) continue;
                int value = nearby[idx].grid_idx;

                for(int l = 0; nearby[idx + l].grid_idx == value && l < particles_count; ++l) {
                    int particle_idx = nearby[idx + l].particle_idx;

                    vec3 particle_diff = predicted_particles[particle_idx].xyz - particle.xyz;
                    if(particle_diff.x == 0 && particle_diff.y == 0 && particle_diff.z == 0){
                        continue;
                    }

                    float d = length(particle_diff);
                    float ds = dsmoothing(density_radius, d);

                    // near density smoothing
                    float nds = steeperSmoothing(density_radius, d) * near_pressure_multiplier;

                    // if moving in same direction pick a random one instead
                    vec3 dir = d == 0 ? random_dir(float(gl_GlobalInvocationID.x + particle_idx)) : particle_diff / d;

                    // instead of just p0 use the average particle pressure between the
                    // current particle and this particle
                    float p0 = target_density - predicted_particles[particle_idx].w;
                    float p1 = target_density - particle.w;
                    float avgp = (p0 + p1) / 2.0;

                    pressure += dir * avgp * ds;
                    //pressure += dir * avgp * nds * .01;
                }
            }
        }
    }

    return vec3(pressure) * particle_mass;
}

vec3 viscosity_force(vec3 particle, vec3 velocity) {
    dvec3 viscosity = dvec3(0.,0.,0.);
    ivec3 base = ivec3(particle / density_radius);

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int idx = nearby_idx[hash_function(base + ivec3(i,j,k))];

                // this means that no such particle exists at those coordinates
                if(idx == INT_MAX) continue;
                int value = nearby[idx].grid_idx;

                for(int l = 0; nearby[idx + l].grid_idx == value && l < particles_count; ++l) {
                    int particle_idx = nearby[idx + l].particle_idx;

                    vec3 particle_diff = particle - predicted_particles[particle_idx].xyz;
                    if(particle_diff.x == 0 && particle_diff.y == 0 && particle_diff.z == 0){
                        continue;
                    }
                    float d = length(particle_diff);
                    float s = smoothing(density_radius, d);
                    viscosity += (velocities[i].xyz - velocity) * s;
                }
            }
        }
    }

    return vec3(viscosity);
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= particles_count) return;

    // add particle pressure forces
    // tells the particle how fast it should conform to target density
    vec3 pressure_accel = pressure_force(predicted_particles[i]) * pressure_multiplier / predicted_particles[i].w;
    velocities[i] += vec4(pressure_accel * dt, 0.);

    // add viscosity force
    // creates friction between nearby particles so that particles within
    // the radius have similar velocities
    vec3 viscosity_accel = viscosity_force(predicted_particles[i].xyz, velocities[i].xyz);
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