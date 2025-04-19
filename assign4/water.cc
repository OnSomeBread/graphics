#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>

#include "proj4.h"

using std::max;
using std::pow;
using std::unordered_map;
using std::vector;

// PROBLEMS
// STARTING POS IS AGGRESSIVE
// EDGES HAVE GAPS
// CLUMPS FORM

// TO INTERACT WITH NEW OBJS ADD TO SPATIAL LOOK UP TABLE

float random_float(float low, float high) {
    return low + static_cast<float>(rand()) /
                     (static_cast<float>(RAND_MAX / (high - low)));
}

V3 random_dir() {
    return {random_float(-1, 1), random_float(-1, 1), random_float(-1, 1)};
}

float magnitude(V3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

// cubic smoothing function
float smoothing(float radius, float diff) {
    float volume = M_PI * pow(radius, 8) / 4;
    return pow(max(0.0f, radius * radius - diff * diff), 3) / volume;
}

// derivative of the smoothing function
float dsmoothing(float radius, float diff) {
    float volume = M_PI * pow(radius, 8) / -24;
    return pow(max(0.0f, radius * radius - diff * diff), 2) * diff / volume;
}

vector<V3> get_nearest_particles(unordered_map<int, vector<V3>>& grid, V3 p,
                                 float density_radius) {
    vector<V3> ans;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; ++k) {
                V3 idx = p / density_radius;

                int grid_idx = idx.x + i * density_radius;
                grid_idx <<= 8;
                grid_idx += idx.y + j * density_radius;
                grid_idx <<= 8;
                grid_idx += idx.z + k * density_radius;
                grid_idx <<= 8;

                ans.insert(ans.end(), grid[grid_idx].begin(),
                           grid[grid_idx].end());
            }
        }
    }
    return ans;
}

// calculates how aggressive the pressure force value should be in order to
// reach target density
float pressure_from_density(float density, float t_density, float p_mult) {
    return (t_density - density) * p_mult;
}

// // add all of the values from the smoothing function in relation to all other
// // particles including itself to prevent density = 0
float calc_density(unordered_map<int, vector<V3>>& grid, vector<V3>& particles,
                   int pointidx, float radius, float mass) {
    float density = 0;

    vector<V3> nearest =
        get_nearest_particles(grid, particles[pointidx], radius);

    for (int i = 0; i < nearest.size(); ++i) {
        float d = magnitude(nearest[i] - particles[pointidx]);
        density += mass * smoothing(radius, d);
    }
    return density;
}

// calculate the pressure force for the particle
V3 pressure_force(unordered_map<int, vector<V3>>& grid, vector<V3>& particles,
                  int pointidx, vector<float>& densities, float target_density,
                  float pressure_mult, float radius, float mass) {
    V3 pressure;

    vector<V3> nearest =
        get_nearest_particles(grid, particles[pointidx], radius);

    for (int i = 0; i < nearest.size(); ++i) {
        // cant use the current particle otherwise creates nans
        if (i == pointidx) {
            continue;
        }

        V3 particle_diff = nearest[i] - particles[pointidx];
        float d = magnitude(particle_diff);
        float ds = dsmoothing(radius, d);

        // if moving in same direction pick a random one instead
        V3 dir = d == 0 ? random_dir() : particle_diff / d;

        // instead of just p0 use the average particle pressure between the
        // current particle and this particle
        float p0 =
            pressure_from_density(densities[i], target_density, pressure_mult);

        float p1 =
            pressure_from_density(densities[i], target_density, pressure_mult);

        float avgp = (p0 + p1) / 2.0;

        pressure += dir * avgp * mass * ds / densities[i];
    }
    return pressure;
}

// add all of the values from the smoothing function in relation to all other
// particles including itself to prevent density = 0
float calc_density(vector<V3>& particles, int pointidx, float radius,
                   float mass) {
    float density = 0;
    for (int i = 0; i < particles.size(); ++i) {
        float d = magnitude(particles[i] - particles[pointidx]);
        density += mass * smoothing(radius, d);
    }
    return density;
}

// calculate the pressure force for the particle
V3 pressure_force(vector<V3>& particles, int pointidx, vector<float>& densities,
                  float target_density, float pressure_mult, float radius,
                  float mass) {
    V3 pressure;
    for (int i = 0; i < particles.size(); ++i) {
        // cant use the current particle otherwise creates nans
        if (i == pointidx) {
            continue;
        }

        V3 particle_diff = particles[i] - particles[pointidx];
        float d = magnitude(particle_diff);
        float ds = dsmoothing(radius, d);

        // if moving in same direction pick a random one instead
        V3 dir = d == 0 ? random_dir() : particle_diff / d;

        // instead of just p0 use the average particle pressure between the
        // current particle and this particle
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

// used to create a t value for interpolate ie a value between 0-1
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
    cout << "CameraFOV 65" << endl;

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
    vector<float> densities;

    int rows = 6;
    int cols = 6;
    int planes = 6;

    // min and max of the boundary box that the particles should be contained in
    V3 min_bound = {0, 0, 0};
    V3 max_bound = {20, 20, 20};
    V3 bound_size = {max_bound.x - min_bound.x, max_bound.y - min_bound.y,
                     max_bound.z - min_bound.z};

    V3 extra = {0, 5, 5};

    V3 pos = min_bound;
    V3 spacing = {(bound_size.x - extra.x) / rows,
                  (bound_size.y - extra.y) / cols,
                  (bound_size.z - extra.z) / planes};

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
                predicted_particles.push_back(pos);
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

    float density_radius = 1.2;
    float particle_mass = 1;

    float target_density = 1.5;

    // how fast do we want particles to be target_density
    float pressure_multiplier = 100;

    int frame_num = 0;
    int total_frames = 1000;

    while (1) {
        cout << "FrameBegin " << frame_num << "\nWorldBegin\n";
        // cout << "FarLight -1.0 0.0 -1.0 1.0 1.0 1.0 1.0\n";
        //  cout << "ObjectInstance \"Axis\"\n";
        //   cout << "Surface \"matte\"\n";

        // auto start = std::chrono::high_resolution_clock::now();
        float fast_v = 0;
        // add gravitational forces to particles
        for (int i = 0; i < particles.size(); ++i) {
            velocities[i].z += -g * dt;
            predicted_particles[i] = particles[i] + velocities[i] * (1 / 120.0);
        }

        // massive optimization to allow for more particles
        // split the bounding box into grid squares of size density_radius
        // then when calculating densities or pressure forces only consider the
        // 3x3 grid of cells since the rest of the particles will add 0 anyway
        // bits 0-7 rows, bits 7-15 cols, and bits 16-23 is planes
        // unordered_map<int, vector<V3>> grid;

        // // populate the particle grid
        // for (int i = 0; i < predicted_particles.size(); ++i) {
        //     V3 idx = predicted_particles[i] / density_radius;

        //     int grid_idx = idx.x;
        //     grid_idx <<= 8;
        //     grid_idx += idx.y;
        //     grid_idx <<= 8;
        //     grid_idx += idx.z;
        //     grid_idx <<= 8;

        //     grid[grid_idx].push_back(predicted_particles[i]);
        // }

        // vector<V3> nearest = get_nearest_particles(
        //     grid_mapping, predicted_particles[16], density_radius);
        // predicted_particles[16].p();
        // for (int i = 0; i < nearest.size(); ++i) {
        //     nearest[i].p();
        // }

        // for (auto itr = grid_mapping.begin(); itr != grid_mapping.end();
        //      ++itr) {
        //     cout << itr->first << endl;
        //     for (int i = 0; i < itr->second.size(); ++i) {
        //         itr->second[i].p();
        //     }
        // }

        for (int i = 0; i < particles.size(); ++i) {
            densities[i] = calc_density(predicted_particles, i, density_radius,
                                        particle_mass);
        }

        // add particle pressure forces
        for (int i = 0; i < particles.size(); ++i) {
            V3 pressure_accel =
                pressure_force(predicted_particles, i, densities,
                               target_density, pressure_multiplier,
                               density_radius, particle_mass) /
                densities[i];

            velocities[i] += pressure_accel * dt;

            fast_v = max(fast_v, magnitude(velocities[i]));
        }

        // auto end = std::chrono::high_resolution_clock::now();
        // auto start2 = std::chrono::high_resolution_clock::now();
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
                 << " " << particles[i].z << "\n";
            cout << "Sphere " << sphere_size << " " << -sphere_size << " "
                 << sphere_size << " 360\nXformPop\n";
        }
        // auto end2 = std::chrono::high_resolution_clock::now();

        // auto duration =
        //     std::chrono::duration_cast<std::chrono::microseconds>(end -
        //     start);
        // auto duration2 =
        // std::chrono::duration_cast<std::chrono::microseconds>(
        //     end2 - start2);

        // cout << duration.count() << "m" << endl;
        // cout << duration2.count() << "m" << endl;

        cout << "WorldEnd\nFrameEnd\n";
        frame_num++;
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_time));
    }
}