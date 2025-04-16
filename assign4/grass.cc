#include <stdlib.h>
#include <time.h>

#include <vector>

#include "proj4.h"

using std::vector;

// gaussian distribution function but with the difference of -x values also
// making output negative
float gaussian(float xmin, float xmax, float mu = 0, float div = 1) {
    float x = xmin + static_cast<float>(rand()) /
                         (static_cast<float>(RAND_MAX / (xmax - xmin)));
    return 1.0 / (div * sqrt(2.0 * M_PI)) *
           exp(-.5 * pow((x - mu) / div, 2.0)) * (std::signbit(x) ? -1.0 : 1.0);
}

int main() {
    srand(time(NULL));
    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 9 9 3" << endl;
    cout << "CameraEye 0 0 12" << endl;
    cout << "CameraFOV 60" << endl;

    cout << "ObjectBegin \"Axis\"" << endl;
    cout << "Color 1 0 0" << endl;
    cout << "Line 0 0 0 5 0 0" << endl;
    cout << "Color 0 1 0" << endl;
    cout << "Line 0 0 0 0 5 0" << endl;
    cout << "Color 0 0 1" << endl;
    cout << "Line 0 0 0 0 0 5" << endl;
    cout << "ObjectEnd # Axis" << endl;

    cout << "WorldBegin" << endl;

    // cout << "ObjectInstance \"Axis\"" << endl;

    cout << "Color .65 .95 .05" << endl;

    // cout << "FarLight -1.0 0.0 -1.0 1.0 1.0 1.0 1.0" << endl;
    // cout << "FarLight 1.0 0.0 -1.0 1.0 1.0 1.0 1.0" << endl;

    int sizex = 70;
    int sizey = 70;

    float spacing = .5;
    float spacing_diff = .5;
    V3 pos = {0, 0, 0};
    for (int i = 0; i < sizex; ++i) {
        pos.y = i * spacing + spacing_diff * gaussian(-3, 3);
        pos.x = 0;
        for (int j = 0; j < sizey; ++j) {
            pos.x = j * spacing + spacing_diff * gaussian(-3, 3);

            cout << "Curve \"Bezier\" \"P\" 3" << endl;
            cout << pos.x << " " << pos.y << " " << pos.z << endl;
            cout << pos.x << " " << pos.y << " " << pos.z + 2 << endl;
            cout << pos.x << " " << pos.y << " " << pos.z + 4 << endl;

            float cx = 4 * gaussian(-3, 3);
            float cy = 4 * gaussian(-3, 3);
            float cz = 4 * gaussian(-3, 3) + 5;

            cout << pos.x + cx << " " << pos.y + cy << " " << pos.z + cz
                 << endl;

            // cout << "Patch \"Bezier\" \"P\" 2 2" << endl;
            // cout << pos.x - .1 << " " << pos.y + .5 << " " << pos.z << endl;
            // cout << pos.x << " " << pos.y - .5 << " " << pos.z + 1 << endl;
            // cout << pos.x + .1 << " " << pos.y + .5 << " " << pos.z << endl;
            // cout << pos.x << " " << pos.y << " " << pos.z + 4 << endl;
            // cout << pos.x << " " << pos.y << " " << pos.z + 5 << endl;
            // cout << pos.x << " " << pos.y << " " << pos.z + 4 << endl;
            // float changex1 = gaussian(-3, 3) - .01;
            // float changey1 = gaussian(-3, 3);
            // float changez1 = gaussian(-3, 3) + 4;
            // cout << pos.x + changex1 << " " << pos.y + changey1 << " "
            //  << pos.z + changez1 << endl;
            // float changex2 = gaussian(-3, 3);
            // float changey2 = gaussian(-3, 3);
            // float changez2 = gaussian(-3, 3) + 4;
            // cout << pos.x + changex2 << " " << pos.y + changey2 << " "
            //  << pos.z + changez2 << endl;
            // float changex3 = gaussian(-3, 3) + .01;
            // float changey3 = gaussian(-3, 3);
            // float changez3 = gaussian(-3, 3) + 4;
            // cout << pos.x + changex3 << " " << pos.y + changey3 << " "
            //  << pos.z + changez3 << endl;
        }
    }

    cout << "WorldEnd" << endl;
}