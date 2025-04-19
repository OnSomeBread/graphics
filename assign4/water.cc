#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <thread>

#include "proj4.h"

using std::max;
using std::pow;
using std::vector;

float random_float(float low, float high) {
    return low + static_cast<float>(rand()) /
                     (static_cast<float>(RAND_MAX / (high - low)));
}

V3 random_dir() {
    return {random_float(-1, 1), random_float(-1, 1), random_float(-1, 1)};
}

float magnitude(V3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

// cubic smoothing
float smoothing(float radius, float diff) {
    float volume = M_PI * pow(radius, 8) / 4;
    return pow(max(0.0f, radius * radius - diff * diff), 3) / volume;
}

// derivative of the smoothing function
float dsmoothing(float radius, float diff) {
    float volume = M_PI * pow(radius, 8) / -24;
    return pow(max(0.0f, radius * radius - diff * diff), 2) * diff / volume;
}

float calc_density(vector<V3>& particles, int pointidx, float radius,
                   float mass) {
    float density = 0;
    for (int i = 0; i < particles.size(); ++i) {
        float d = magnitude(particles[i] - particles[pointidx]);
        density += mass * smoothing(radius, d);
    }
    return density;
}

// V3 calc_gradient(vector<V3>& particles, V3 p, float radius, float mass) {
//     float step = 0.001;
//     V3 px = {step, 0, 0};
//     V3 py = {0, step, 0};
//     V3 pz = {0, 0, step};

//     float di = calc_density(particles, p, radius, mass);
//     float dx = calc_density(particles, p + px, radius, mass) - di;
//     float dy = calc_density(particles, p + py, radius, mass) - di;
//     float dz = calc_density(particles, p + pz, radius, mass) - di;

//     return {dx / step, dy / step, dz / step};
// }

float pressure_from_density(float density, float t_density, float p_mult) {
    return (t_density - density) * p_mult;
}

V3 pressure_force(vector<V3>& particles, int pointidx, vector<float>& densities,
                  float target_density, float pressure_mult, float radius,
                  float mass) {
    V3 pressure;
    for (int i = 0; i < particles.size(); ++i) {
        if (i == pointidx) {
            continue;
        }

        V3 particle_diff = particles[i] - particles[pointidx];
        float d = magnitude(particle_diff);
        float ds = dsmoothing(radius, d);

        V3 dir = d == 0 ? random_dir() : particle_diff / d;
        float p0 =
            pressure_from_density(densities[i], target_density, pressure_mult);

        float p1 =
            pressure_from_density(densities[i], target_density, pressure_mult);

        float avgp = (p0 + p1) / 2.0;

        pressure += dir * avgp * mass * ds / densities[i];
    }
    return pressure;
}

// changes particle pos and velocity if out of bounds
void bounds_check(V3& point, V3& v, float damping, V3 min_pos, V3 max_pos) {
    if (point.x < min_pos.x) {
        point.x = min_pos.x;
        v = v * -damping;
    }
    if (point.x > max_pos.x) {
        point.x = max_pos.x;
        v = v * -damping;
    }

    if (point.y < min_pos.y) {
        point.y = min_pos.y;
        v = v * -damping;
    }
    if (point.y > max_pos.y) {
        point.y = max_pos.y;
        v = v * -damping;
    }

    if (point.z < min_pos.z) {
        point.z = min_pos.z;
        v = v * -damping;
    }
    if (point.z > max_pos.z) {
        point.z = max_pos.z;
        v = v * -damping;
    }
}

float scale_t_val(float value, float data_min, float data_max) {
    return (value - data_min) / (data_max - data_min);
}

V3 interpolate(V3 start, V3 end, float t) { return (end - start) * t + start; }

int main() {
    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 12 0 8" << endl;
    cout << "CameraEye -4 -20 16" << endl;
    cout << "CameraFOV 70" << endl;

    cout << "ObjectBegin \"Axis\"" << endl;
    cout << "Color 1 0 0" << endl;
    cout << "Line 0 0 0 5 0 0" << endl;
    cout << "Color 0 1 0" << endl;
    cout << "Line 0 0 0 0 5 0" << endl;
    cout << "Color 0 0 1" << endl;
    cout << "Line 0 0 0 0 0 5" << endl;
    cout << "ObjectEnd # Axis" << endl;

    std::srand(std::time(0));

    vector<V3> particles;
    vector<V3> velocities;
    vector<float> densities;

    int rows = 5;
    int cols = 5;
    int planes = 5;

    // min and max of the boundary box that the particles should be contained in
    V3 min_bound = {0, 0, 0};
    V3 max_bound = {20, 20, 20};

    V3 extra = {10, 10, 10};

    V3 pos = min_bound;
    V3 spacing = {(max_bound.x - min_bound.x - extra.x) / rows,
                  (max_bound.y - min_bound.y - extra.y) / cols,
                  (max_bound.z - min_bound.z - extra.z) / planes};

    for (int i = 0; i < rows; ++i) {
        pos.x = i * spacing.x;
        for (int j = 0; j < cols; ++j) {
            pos.y = j * spacing.y;
            for (int k = 0; k < planes; ++k) {
                pos.x += random_float(-spacing.x / 2.0, spacing.x / 2.0);
                pos.y += random_float(-spacing.y / 2.0, spacing.y / 2.0);
                pos.z = k * spacing.z +
                        random_float(-spacing.z / 2.0, spacing.z / 2.0);
                particles.push_back(pos);
                velocities.push_back({0, 0, 0});
                densities.push_back(0);
            }
        }
    }

    float g = 1;
    int target_fps = 60;
    int frame_time = 1000 / target_fps;
    float dt = 1 / (float)target_fps;

    float sphere_size = .5;
    V3 slow_particle_color = {0, 1, 1};
    V3 fast_particle_color = {1, 0, 0};

    float particle_damping = .5;

    float density_radius = 1;
    float particle_mass = 1;

    float target_density = 5;

    // how fast do we want particles to be target_density
    float pressure_multiplier = 50;

    int frame_num = 0;
    while (1) {
        cout << "FrameBegin " << frame_num << "\nWorldBegin\n";
        cout << "FarLight -1.0 0.0 -1.0 1.0 1.0 1.0 1.0" << endl;
        cout << "ObjectInstance \"Axis\"\n";
        cout << "Surface \"matte\"\n";

        float fast_v = 0;
        for (int i = 0; i < particles.size(); ++i) {
            velocities[i].z += -g * dt;
            densities[i] =
                calc_density(particles, i, density_radius, particle_mass);
        }

        for (int i = 0; i < particles.size(); ++i) {
            V3 pressure_accel =
                pressure_force(particles, i, densities, target_density,
                               pressure_multiplier, density_radius,
                               particle_mass) /
                densities[i];

            velocities[i] += pressure_accel * dt;

            fast_v = max(fast_v, magnitude(velocities[i]));
        }

        for (int i = 0; i < particles.size(); ++i) {
            particles[i] += velocities[i] * dt;
            bounds_check(particles[i], velocities[i], particle_damping,
                         min_bound, max_bound);

            cout << "XformPush\n";
            V3 particle_color =
                interpolate(slow_particle_color, fast_particle_color,
                            scale_t_val(magnitude(velocities[i]), 0, fast_v));
            cout << "Color " << particle_color.x << " " << particle_color.y
                 << " " << particle_color.z << " \n";
            cout << "Translate " << particles[i].x << " " << particles[i].y
                 << " " << particles[i].z << "\nSphere " << sphere_size << " "
                 << -sphere_size << " " << sphere_size << " 360\nXformPop\n ";
        }

        cout << "WorldEnd\nFrameEnd\n";
        frame_num++;
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_time));
    }
}