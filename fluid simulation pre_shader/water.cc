#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <chrono>
#include <cmath>
#include <thread>
#include <unordered_map>
#include <vector>

#include "marching_cubes.cc"

using std::max;
using std::pow;
using std::round;
using std::sqrt;
using std::unordered_map;
using std::vector;

// PROBLEMS
// STARTING POS IS AGGRESSIVE
// EDGES HAVE GAPS
// CLUMPS FORM
// IMPLEMENT MARCHING CUBES
// ADD NEAR DENSITY AND NEAR PRESSURE FORCES
// https://sph-tutorial.physics-simulation.org/pdf/SPH_Tutorial.pdf

// massive optimization to allow for more particles
// split the bounding box into grid squares of size density_radius
// then when calculating densities or pressure forces only consider the
// 3x3 grid of cells since the rest of the particles will add 0 anyway
// bits 0-7 planes, bits 7-15 cols, and bits 16-23 is rows
unordered_map<int, vector<V3>> grid;
int grid_size = 1000;

// the spatial
int hash_function(int x, int y, int z) {
    return ((x * 73856093) xor (y * 19349663) xor (z * 83492791)) % grid_size;
}

float magnitude(V3 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

// cubic smoothing function
double smoothing(double radius, double diff) {
    if (diff < radius) {
        double volume = (64 * M_PI * pow(radius, 9)) / 315.0;
        return pow(radius * radius - diff * diff, 3) / volume;
    }
    return 0;
}

float steeperSmoothing(float dst, float radius) {
    if (dst < radius) {
        double volume = (M_PI * pow(radius, 6)) / 15.0;
        return pow(radius - dst, 3) * volume;
    }
    return 0;
}

// derivative of the smoothing function
double dsmoothing(double radius, double diff) {
    if (diff < radius) {
        double volume = (64 * M_PI * pow(radius, 9)) / 315.0;
        return -6 * pow(radius * radius - diff * diff, 2) * diff / volume;
    }
    return 0;
}

float DsteeperSmoothing(float dst, float radius) {
    if (dst <= radius) {
        double volume = (pow(radius, 6) * M_PI) / 35.0;
        return -pow(radius - dst, 2) / volume;
    }
    return 0;
}

vector<V3> get_nearest_particles(V3 p, float density_radius) {
    vector<V3> nearby;
    V3 idx = p / density_radius;

    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; ++k) {
                int grid_idx = hash_function((int)idx.x + i, (int)idx.y + j,
                                             (int)idx.z + k);
                nearby.insert(nearby.end(), grid[grid_idx].begin(),
                              grid[grid_idx].end());
            }
        }
    }
    return nearby;
}

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
double calc_density(vector<V3>& particles, V3 particle, float radius,
                    float mass) {
    double density = 0;
    for (int i = 0; i < (int)particles.size(); ++i) {
        float d = magnitude(particles[i] - particle);
        density += mass * smoothing(radius, d);
    }
    return density;
}

// calculate the pressure force for the particle
V3 pressure_force(vector<V3>& particles, int pointidx,
                  vector<double>& densities, float target_density, float radius,
                  float mass) {
    V3 pressure;

    for (int i = 0; i < (int)particles.size(); ++i) {
        // cant use the current particle otherwise creates nans
        if (i == pointidx) continue;

        V3 particle_diff = particles[i] - particles[pointidx];
        float d = magnitude(particle_diff);
        float ds = dsmoothing(radius, d);

        // if moving in same direction pick a random one instead
        V3 dir = d == 0 ? random_dir() : particle_diff / d;

        // instead of just p0 use the average particle pressure between the
        // current particle and this particle
        float p0 = (target_density - densities[i]);
        float p1 = (target_density - densities[pointidx]);
        float avgp = (p0 + p1) / 2.0;

        pressure += dir * avgp * mass * ds / densities[i];
    }
    return pressure;
}

V3 viscosity_force(vector<V3>& particles, int pointidx, vector<V3>& velocities,
                   float radius) {
    V3 viscosity;
    for (int i = 0; i < (int)particles.size(); ++i) {
        if (i == pointidx) continue;
        float d = magnitude(particles[pointidx] - particles[i]);
        float s = smoothing(radius, d);
        viscosity += (velocities[i] - velocities[pointidx]) * s;
    }

    return viscosity;
}

// used to create a t value for interpolate ie a value between 0-1
float scale_t_val(float value, float data_min, float data_max) {
    return (value - data_min) / (data_max - data_min);
}

// an optimization idea change translate to be diff between particles so no need
// for xformpush or xformpops
// V3 change_origin(V3 origin, V3 p) { return (p - origin) + origin; }

int main() {
    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 5 0 6" << endl;
    cout << "CameraEye -4 -15 16" << endl;
    cout << "CameraFOV 40" << endl;

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
    vector<V3> predicted_particles;
    vector<double> densities;

    int rows = 9;
    int cols = 9;
    int planes = 9;

    // min and max of the boundary box that the particles should be contained in
    V3 min_bound = {0, 0, 0};
    V3 max_bound = {21, 21, 21};
    V3 bound_size = {max_bound.x - min_bound.x, max_bound.y - min_bound.y,
                     max_bound.z - min_bound.z};

    V3 extra = {0, 5, 5};

    V3 pos = min_bound;
    V3 spacing = {(bound_size.x - extra.x) / rows,
                  (bound_size.y - extra.y) / cols,
                  (bound_size.z - extra.z) / planes};

    // make a particle grid with some randomization in how they are placed along
    // the grid
    for (int i = 0; i < rows; ++i) {
        pos.x = i * spacing.x;
        for (int j = 0; j < cols; ++j) {
            pos.y = j * spacing.y;
            for (int k = 0; k < planes; ++k) {
                pos.x += random_float(-spacing.x / 3.0, spacing.x / 3.0);
                pos.y += random_float(-spacing.y / 3.0, spacing.y / 3.0);
                pos.z = k * spacing.z +
                        random_float(-spacing.z / 3.0, spacing.z / 3.0);
                particles.push_back(pos);
                predicted_particles.push_back(pos);
                velocities.push_back({0, 0, 0});
                densities.push_back(0);
            }
        }
    }

    float sphere_size = .4;
    V3 slow_particle_color = {.1, .25, 1};
    V3 fast_particle_color = {.25, .55, .95};
    float fast_v = 4;  // highest value for color

    double g = .8;
    double particle_mass = 1;

    double particle_damping = .05;
    double density_radius = 1.9;
    double target_density = 2;

    // for the marching cubes algo

    float surfacelvl = .2;
    float field_size = 2;
    int field_rows = bound_size.x / field_size + 1.0f;
    int field_cols = bound_size.y / field_size + 1.0f;
    int field_planes = bound_size.z / field_size + 1.0f;
    vector<vector<vector<float>>> data(
        field_rows,
        vector<vector<float>>(field_cols, vector<float>(field_planes, 0)));

    // how fast do we want particles to be target_density
    float pressure_multiplier = 30;
    float viscosity_multiplier = .4;

    long unsigned frame_num = 0;
    int frame_count = 1000;
    auto last_frame_time = std::chrono::high_resolution_clock::now();
    while (--frame_count) {
        auto curr = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        curr - last_frame_time)
                        .count() /
                    1000000.0f;

        last_frame_time = curr;

        cout << "FrameBegin " << frame_num << " WorldBegin ";
        cout << "AmbientLight 0.6 0.7 0.8 0.8 ";
        cout << "FarLight -1.0 0.0 -1.0 1.0 1.0 1.0 1.0 ";
        cout << "PointLight -5 -5 -5 1 1 0 1 ";
        // cout << "ObjectInstance \"Axis\"\n";
        // cout << "Surface \"plastic\"\n";

        // auto start = std::chrono::high_resolution_clock::now();
        // add gravitational forces to particles
        for (int i = 0; i < (int)particles.size(); ++i) {
            velocities[i].z += -g * dt;
            predicted_particles[i] = particles[i] + velocities[i] * dt;
        }

        // populate the particle grid
        grid.clear();
        for (int i = 0; i < (int)predicted_particles.size(); ++i) {
            V3 idx = predicted_particles[i] / density_radius;
            int grid_idx = hash_function((int)idx.x, (int)idx.y, (int)idx.z);
            grid[grid_idx].push_back(predicted_particles[i]);
        }

        // create densities table for it to be used in pressure forces
        for (int i = 0; i < (int)particles.size(); ++i) {
            vector<V3> nearby =
                get_nearest_particles(predicted_particles[i], density_radius);
            densities[i] = calc_density(nearby, predicted_particles[i],
                                        density_radius, particle_mass);
        }

        // add particle pressure forces
        // tells the particle how fast it should conform to target density
        for (int i = 0; i < (int)particles.size(); ++i) {
            V3 pressure_accel =
                pressure_force(predicted_particles, i, densities,
                               target_density, density_radius, particle_mass) *
                pressure_multiplier / densities[i];

            velocities[i] += pressure_accel * dt;
        }

        // add viscosity force
        // creates friction between nearby particles so that particles within
        // the radius have similar velocities
        for (int i = 0; i < (int)particles.size(); ++i) {
            velocities[i] +=
                viscosity_force(particles, i, velocities, density_radius) *
                viscosity_multiplier;
        }

        for (int i = 0; i < (int)particles.size(); ++i) {
            particles[i] += velocities[i] * dt;
            bounds_check(particles[i], velocities[i], particle_damping,
                         min_bound, max_bound);
        }

        // for (int i = 0; i < field_rows; ++i) {
        //     for (int j = 0; j < field_cols; ++j) {
        //         for (int k = 0; k < field_planes; ++k) {
        //             V3 field_particle = {
        //                 (float)i * field_size + field_size * .5,
        //                 (float)j * field_size + field_size * .5,
        //                 (float)k * field_size + field_size * .5};

        //             vector<V3> nearby =
        //                 get_nearest_particles(field_particle,
        //                 density_radius);
        //             data[i][j][k] = calc_density(nearby, field_particle,
        //                                          density_radius,
        //                                          particle_mass);
        //         }
        //     }
        // }

        // marching_cubes(data, surfacelvl);

        for (int i = 0; i < (int)particles.size(); ++i) {
            V3 particle_color =
                interpolate(slow_particle_color, fast_particle_color,
                            scale_t_val(magnitude(velocities[i]), 0, fast_v));

            cout << "Color " << particle_color.x << " " << particle_color.y
                 << " " << particle_color.z << " ";

            cout << "XformPush Translate " << particles[i].x << " "
                 << particles[i].y << " " << particles[i].z << " ";
            cout << "Scale " << sphere_size << " " << sphere_size << " "
                 << sphere_size << " ";
            cout << "Cube XformPop ";

            // cout << "Sphere " << sphere_size << " " << -sphere_size << " "
            //      << sphere_size << " 360\nXformPop\n";
        }

        cout << "WorldEnd FrameEnd ";
        frame_num++;
    }
}

// changes particle pos and velocity if out of bounds
void bounds_check(V3& point, V3& v, float damping, V3 min_pos, V3 max_pos) {
    if (point.x < min_pos.x) {
        point.x = min_pos.x;
        v.x = v.x * -damping;
    }
    if (point.x > max_pos.x) {
        point.x = max_pos.x;
        v.x = v.x * -damping;
    }

    if (point.y < min_pos.y) {
        point.y = min_pos.y;
        v.y = v.y * -damping;
    }
    if (point.y > max_pos.y) {
        point.y = max_pos.y;
        v.y = v.y * -damping;
    }

    if (point.z < min_pos.z) {
        point.z = min_pos.z;
        v.z = v.z * -damping;
    }
    if (point.z > max_pos.z) {
        point.z = max_pos.z;
        v.z = v.z * -damping;
    }
}