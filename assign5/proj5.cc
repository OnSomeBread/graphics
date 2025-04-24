#include "proj5.h"

#include <stdlib.h>
#include <time.h>

#include <cmath>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::pow;
using std::vector;

// gaussian distribution function
float gaussian(float div = 1, float xmin = -3, float xmax = 3, float mu = 0) {
    float x = xmin + static_cast<float>(rand()) /
                         (static_cast<float>(RAND_MAX / (xmax - xmin)));
    return 1.0 / (div * sqrt(2.0 * M_PI)) * exp(-.5 * pow((x - mu) / div, 2.0));
}

float f3(float delta, int x0, int x1, int x2) {
    return (x0 + x1 + x2) / 3 + delta * gaussian();
}

float f4(float delta, int x0, int x1, int x2, int x3) {
    return (x0 + x1 + x2 + x3) / 4 + delta * gaussian();
}

int main() {
    int maxlevel = 10;
    int sigma = 1;
    srand(time(NULL));

    int N = pow(2, maxlevel);
    vector<vector<float>> X(N + 1, vector<float>(N + 1));

    int y0 = 0;
    float delta = sigma;
    X[0][0] = delta * gaussian();
    X[0][N] = delta * gaussian();
    X[N][0] = delta * gaussian();
    X[N][N] = delta * gaussian();
    int D = N;
    int d = N / 2;

    for (int stage = 1; stage < maxlevel; ++stage) {
        /* going from grid type I to type II */
        delta *= pow(0.5, 0.5 * (3 - D));

        /* interpolate and offset mid points */
        for (int x = d; x < N - d; x += D) {
            for (int y = d; y < N - d; y += D) {
                X[x][y] = f4(delta, X[x + d][y + d], X[x + d][y - d],
                             X[x - d][y + d], X[x - d][y - d]);
            }
        }
        /* displace existing points */
        for (int x = 0; x < N; x += D) {
            for (int y = 0; y < N; y += D) {
                X[x][y] = X[x][y] + delta * gaussian();
            }
        }

        /* going from grid type II to type I */
        delta = delta * pow(0.5, 0.5 * (3 - D));

        /* interpolate and offset mid points at boundary */
        for (int x = d; x < N - d; x += D) {
            X[x][0] = f3(delta, X[x + d][0], X[x - d][0], X[x][d]);
            X[x][N] = f3(delta, X[x + d][N], X[x - d][N], X[x][N - d]);
            X[0][x] = f3(delta, X[0][x + d], X[0][x - d], X[d][x]);
            X[N][x] = f3(delta, X[N][x + d], X[N][x - d], X[N - d][x]);
        }

        /* interpolate and offset mid points in interior */
        for (int x = d; x < N - d; x += D) {
            for (int y = D; y < N - d; y += D) {
                X[x][y] = f4(delta, X[x][y + d], X[x][y - d], X[x + d][y],
                             X[x - d][y]);
            }
        }

        for (int x = D; x < N - d; x += D) {
            for (int y = d; y < N - d; y += D) {
                X[x][y] = f4(delta, X[x][y + d], X[x][y - d], X[x + d][y],
                             X[x - d][y]);
            }
        }

        /* displace existing points */
        for (int x = 0; x < N; x += D) {
            for (int y = 0; y < N; y += D) {
                X[x][y] = X[x][y] + delta * gaussian();
            }
        }
        for (int x = d; x < N - d; x += D) {
            for (int y = d; y < N - d; y += D) {
                X[x][y] = X[x][y] + delta * gaussian();
            }
        }

        /* prepare for next level */
        D = D / 2.0;
        d = d / 2.0;
    }

    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 0 0 10" << endl;
    cout << "CameraEye -4 -25 4" << endl;
    cout << "CameraFOV 60" << endl;

    // create object Axis
    cout << "ObjectBegin \"Axis\"" << endl;
    cout << "Color 1 0 0" << endl;
    cout << "Line 0 0 0 5 0 0" << endl;
    cout << "Color 0 1 0" << endl;
    cout << "Line 0 0 0 0 5 0" << endl;
    cout << "Color 0 0 1" << endl;
    cout << "Line 0 0 0 0 0 5" << endl;
    cout << "ObjectEnd # Axis" << endl;

    cout << "WorldBegin" << endl;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << "" << endl;
        }
    }

    cout << "WorldEnd" << endl;
}