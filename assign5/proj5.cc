#include "proj5.h"

double random_float(double low, double high) {
    return low + static_cast<double>(rand()) /
                     (static_cast<double>(RAND_MAX / (high - low)));
}

// gaussian distribution function
double gaussian(double low = -3, double high = 3, double mu = 0,
                double div = 1) {
    double x = random_float(low, high);
    return 1.0 / (div * sqrt(2.0 * M_PI)) *
           exp(-.5 *
               pow((x - mu) / div, 2.0));  // * (std::signbit(x) ? -1.0 : 1.0)
}

// find the mid point in a triangle with small random factor
int f3(double delta, double x0, double x1, double x2) {
    return (x0 + x1 + x2) / 3.0 + delta * gaussian();
}

// find the mid point in a square with small random factor
double f4(double delta, double x0, double x1, double x2, double x3) {
    return (x0 + x1 + x2 + x3) / 4.0 + delta * gaussian();
}

void MidPointFM2D(vector<vector<double>>& X, int maxlevel, double sigma,
                  double H, int seed = time(NULL)) {
    srand(seed);
    int N = pow(2, maxlevel);
    int D = N;
    int d = N / 2;

    double delta = sigma;
    X[0][0] = delta * gaussian();
    X[0][N] = delta * gaussian();
    X[N][0] = delta * gaussian();
    X[N][N] = delta * gaussian();

    for (int stage = 1; stage <= maxlevel; ++stage) {
        /* going from grid type I to type II */
        delta *= pow(0.5, 0.5 * H);

        /* interpolate and offset mid points */
        for (int x = d; x <= N - d; x += D) {
            for (int y = d; y <= N - d; y += D) {
                X[x][y] = f4(delta, X[x + d][y + d], X[x + d][y - d],
                             X[x - d][y + d], X[x - d][y - d]);
            }
        }

        /* displace existing points */
        for (int x = 0; x <= N; x += D) {
            for (int y = 0; y <= N; y += D) {
                X[x][y] += delta * gaussian();
            }
        }

        /* going from grid type II to type I */
        delta *= pow(0.5, 0.5 * H);

        /* interpolate and offset mid points at boundary */
        for (int x = d; x <= N - d; x += D) {
            X[x][0] = f3(delta, X[x + d][0], X[x - d][0], X[x][d]);
            X[x][N] = f3(delta, X[x + d][N], X[x - d][N], X[x][N - d]);
            X[0][x] = f3(delta, X[0][x + d], X[0][x - d], X[d][x]);
            X[N][x] = f3(delta, X[N][x + d], X[N][x - d], X[N - d][x]);
        }

        /* interpolate and offset mid points in interior */
        for (int x = d; x <= N - d; x += D) {
            for (int y = D; y <= N - d; y += D) {
                X[x][y] = f4(delta, X[x][y + d], X[x][y - d], X[x + d][y],
                             X[x - d][y]);
            }
        }

        for (int x = D; x <= N - d; x += D) {
            for (int y = d; y <= N - d; y += D) {
                X[x][y] = f4(delta, X[x][y + d], X[x][y - d], X[x + d][y],
                             X[x - d][y]);
            }
        }

        /* displace existing points */
        for (int x = 0; x <= N; x += D) {
            for (int y = 0; y <= N; y += D) {
                X[x][y] += delta * gaussian();
            }
        }
        for (int x = d; x <= N - d; x += D) {
            for (int y = d; y <= N - d; y += D) {
                X[x][y] += delta * gaussian();
            }
        }

        /* prepare for next level */
        D /= 2;
        d /= 2;
    }
}

int main() {
    double sigma = 30;
    double D = 2.2;
    int n = 6;

    int N = pow(2, n);
    vector<vector<double>> X(N + 1, vector<double>(N + 1));

    // populate the
    MidPointFM2D(X, n, sigma, 3 - D);

    V3 low_color = {0, 0, 0};
    V3 high_color = {.7, .7, .7};

    // create the polyset coords
    vector<V3> coords;
    float highest = -1e30;
    float lowest = 1e30;
    float spacing = 100.0 / (float)N;
    for (int x = 0; x < (N + 1); ++x) {
        for (int y = 0; y < (N + 1); ++y) {
            coords.push_back(
                {(float)x * spacing, (float)y * spacing, (float)X[x][y]});

            // check to see if it is the highest or lowest height seen so far
            highest = max(highest, (float)X[x][y]);
            lowest = min(lowest, (float)X[x][y]);
        }
    }

    // create the polyset faces
    vector<vector<int>> faceList;
    int faceNum = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            vector<int> face;
            face.push_back(faceNum);
            face.push_back(faceNum + N + 1);
            face.push_back(faceNum + N + 2);
            face.push_back(faceNum + 1);
            faceList.push_back(face);
            faceNum++;
        }
    }

    // print the file that will go into rd_view
    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 50 50 10" << endl;
    cout << "CameraEye 10 -50 100" << endl;
    cout << "CameraFOV 30" << endl;
    // cout << "OptionBool \"DoubleSide\" on" << endl;

    cout << "WorldBegin" << endl;
    cout << "AmbientLight 0.6 0.7 0.8 0.8" << endl;
    cout << "FarLight -1.0 0.0 -1.0 1.0 1.0 1.0 1.0" << endl;
    cout << "PointLight -5 -5 -5 1 1 0 1" << endl;
    cout << "PolySet \"PC\" " << coords.size() << " " << faceList.size()
         << endl;

    for (int i = 0; i < (int)coords.size(); ++i) {
        V3 color =
            interpolate(low_color, high_color,
                        scale_t_val(magnitude(coords[i]), lowest, highest));
        cout << coords[i].x << " " << coords[i].y << " " << coords[i].z << " "
             << color.x << " " << color.y << " " << color.z << endl;
    }

    for (int i = 0; i < (int)faceList.size(); ++i) {
        for (int j = 0; j < (int)faceList[i].size(); ++j) {
            cout << faceList[i][j] << " ";
        }
        cout << "-1" << endl;
    }

    cout << "WorldEnd" << endl;
}

void print_vec(vector<vector<int>>& X) {
    for (int i = 0; i < (int)X.size(); ++i) {
        for (int j = 0; j < (int)X.size(); ++j) {
            cout << X[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// used to create a t value for interpolate ie a value between 0-1
double scale_t_val(double value, double data_min, double data_max) {
    return (value - data_min) / (data_max - data_min);
}

V3 interpolate(V3 start, V3 end, double t) { return (end - start) * t + start; }

float magnitude(V3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }