#include <stdlib.h>
#include <time.h>

#include <deque>
#include <thread>
#include <utility>
#include <vector>

#include "proj4.h"

using std::deque;
using std::pair;
using std::pow;
using std::sqrt;

// gaussian distribution function but with the difference of -x values also
// making output negative
float gaussian(float xmin, float xmax, float mu = 0, float div = 1) {
    float x = xmin + static_cast<float>(rand()) /
                         (static_cast<float>(RAND_MAX / (xmax - xmin)));
    return 1.0 / (div * sqrt(2.0 * M_PI)) *
           exp(-.5 * pow((x - mu) / div, 2.0)) * (std::signbit(x) ? -1.0 : 1.0);
}

V3 interpolate(V3 start, V3 end, float t) { return (end - start) * t + start; }

int main() {
    srand(time(NULL));
    deque<pair<V3, float>> particles;

    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 0 0 10" << endl;
    cout << "CameraEye -4 -25 4" << endl;
    cout << "CameraFOV 60" << endl;

    cout << "ObjectBegin \"Axis\"" << endl;
    cout << "Color 1 0 0" << endl;
    cout << "Line 0 0 0 5 0 0" << endl;
    cout << "Color 0 1 0" << endl;
    cout << "Line 0 0 0 0 5 0" << endl;
    cout << "Color 0 0 1" << endl;
    cout << "Line 0 0 0 0 0 5" << endl;
    cout << "ObjectEnd # Axis" << endl;

    int frame_time = 1000 / 60;
    float particle_intervals = 1.0 / 60.0;

    // end point bounds
    float max_x = 12;
    float max_y = 12;
    float max_z = 20;

    // frame_num % spawn_particle == 0 then create new particle
    int spawn_particle = 2;
    int spawn_amount = 2;

    int frame_num = 0;
    V3 start = {0, 0, 0};
    while (1) {
        // create a new particle
        if (frame_num % spawn_particle == 0) {
            for (int i = 0; i < spawn_amount; ++i) {
                // end particle coord, starting t value of 0
                particles.push_back(
                    {{start.x + max_x * gaussian(-3, 3),
                      start.y + max_y * gaussian(-3, 3), start.z + max_z},
                     0});
            }
        }

        cout << "FrameBegin " << frame_num << "\nWorldBegin\n";

        // cout << "ObjectInstance \"Axis\"" << endl;

        cout << "Color .15 .15 .15\n";

        // draw all of the particles
        for (int i = 0; i < (int)particles.size(); ++i) {
            V3 particle =
                interpolate(start, particles[i].first, particles[i].second);
            // cout << "Opacity " << 1.0 - t_values[i] << endl;

            cout << "XformPush\nTranslate " << particle.x << " " << particle.y
                 << " " << particle.z << "\nSphere 1 -1 1 360\nXformPop\n";
        }

        cout << "WorldEnd\nFrameEnd\n";

        // remove all of the particles that have now reached the end point
        // technically the first condition is not needed as long as there is
        // always a particle on screen
        // while (i < particles.size() && particles[i].second > 1.0)
        int i = 0;
        while (particles[i].second > 1.0) {
            particles.pop_front();
            ++i;
        }

        // update the intervals
        for (i = 0; i < (int)particles.size(); ++i) {
            particles[i].second += particle_intervals;
        }

        ++frame_num;
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_time));
    }
}