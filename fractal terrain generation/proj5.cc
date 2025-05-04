#include "proj5.h"

double random_float(double low, double high) {
    return low + static_cast<double>(rand()) /
                     (static_cast<double>(RAND_MAX / (high - low)));
}

// gaussian distribution function
double gaussian(int n = 4) {
    double s = 0;
    for(int i = 0; i < n; ++i){
        s += rand();
    }

    return 1.0 / (float)RAND_MAX * sqrt(12.0 / (double)n) * s - sqrt(3.0 * (double)n);
}

// find the mid point in a triangle with small random factor
double f3(double delta, double x0, double x1, double x2) {
    return (x0 + x1 + x2) / 3.0 + delta * gaussian();
}

// find the mid point in a square with small random factor
double f4(double delta, double x0, double x1, double x2, double x3) {
    return (x0 + x1 + x2 + x3) / 4.0 + delta * gaussian();
}

void MidPointFM2D(vector<vector<double>>& X, int maxlevel, double sigma,
                  double H, int seed) {
    std::cerr << "seed: " << seed << endl;
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

int main(int argc, char* argv[]) {
    if (argc != 4 && argc != 5) {
        std::cerr << "Usage ./proj5 n D sigma (optional)seed" << endl;
        return 1;
    }
    int n = std::stoi(argv[1]);
    double D = std::stod(argv[2]);
    double sigma = std::stoi(argv[3]);
    int seed = time(NULL);
    if (argc == 5) {
        seed = std::stoi(argv[4]);
    }

    // int n = 5;
    // double D = 2.2;
    // double sigma = 30;

    int N = pow(2, n);
    vector<vector<double>> X(N + 1, vector<double>(N + 1));

    // populate the
    MidPointFM2D(X, n, sigma, 3 - D, seed);

    // vector<vector<V3>> sphere_coords;
    // vector<vector<V3>> normals;

    // int n_divisions = N;
    // float highest_sphere = -1e30;
    // float lowest_sphere = 1e30;
    // for (int i = 0; i < n_divisions + 1; ++i) {
    //     double u = (2.0 * M_PI * i) / (double)n_divisions;
    //     vector<V3> rows;
    //     vector<V3> rows_normals;
    //     for (int j = 0; j < n_divisions + 1; ++j) {
    //         double v = (M_PI * j) / (n_divisions / 2.0) - (M_PI / 2.0);
    //         double r = 50.0 + X[i][j];
    //         V3 p;
    //         p.x = r * cos(v) * cos(u);
    //         p.y = r * cos(v) * sin(u);
    //         p.z = r * sin(v);

    //         // see if its a peak or a valley
    //         float mag = magnitude(p);
    //         highest_sphere = max(highest_sphere, mag);
    //         lowest_sphere = min(lowest_sphere, mag);
    //         rows.push_back(p);

    //         // haha normal time
    //         // partial derivative with respect to u
    //         V3 du_normal;
    //         du_normal.x = r * cos(v) * -sin(u);
    //         du_normal.y = r * cos(v) * cos(u);
    //         du_normal.z = 0;

    //         // partial derivative with respect to v
    //         V3 dv_normal;
    //         dv_normal.x = r * -sin(v) * cos(u);
    //         dv_normal.y = r * -sin(v) * sin(u);
    //         dv_normal.z = r * cos(v);

    //         V3 normal = normalize(cross_product(dv_normal, du_normal));
    //         rows_normals.push_back(normal);
    //     }
    //     sphere_coords.push_back(rows);
    //     normals.push_back(rows_normals);
    // }

    // vector<vector<int>> sphere_faceList;

    // int s = sphere_coords.size();
    // for (int i = 0; i < s - 1; ++i) {
    //     for (int j = 0; j < s - 1; ++j) {
    //         int faceNum = i * s + j;
    //         vector<int> face;
    //         face.push_back(faceNum);
    //         face.push_back(faceNum + s);
    //         face.push_back(faceNum + s + 1);
    //         face.push_back(faceNum + 1);
    //         sphere_faceList.push_back(face);
    //     }
    // }

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

    V3 low_color = {.33, .42, .18};
    V3 high_color = {.74, .72, .42};
    // V3 low_color = {.3, .3, .3};
    // V3 high_color = {.5, .5, .5};

    // create the polyset faces
    vector<vector<int>> faceList;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int faceNum = i * (N + 1) + j;
            vector<int> face;
            face.push_back(faceNum);
            face.push_back(faceNum + N + 1);
            face.push_back(faceNum + N + 2);
            face.push_back(faceNum + 1);
            faceList.push_back(face);
        }
    }

    // print the file that will go into rd_view
    cout << "Display \"Objects\"  \"Screen\"  \"rgbdouble\"" << endl;
    cout << "Background 0.6 0.7 0.8" << endl;
    cout << "CameraUp 0 0 1" << endl;
    cout << "CameraAt 50 50 0" << endl;
    cout << "CameraEye 50 -50 50" << endl;
    cout << "CameraFOV 60" << endl;
    // cout << "OptionBool \"DoubleSide\" on" << endl;

    // axis object
    cout << "ObjectBegin \"Axis\"" << endl;
    cout << "Color 1 0 0" << endl;
    cout << "Line 0 0 0 5 0 0" << endl;
    cout << "Color 0 1 0" << endl;
    cout << "Line 0 0 0 0 5 0" << endl;
    cout << "Color 0 0 1" << endl;
    cout << "Line 0 0 0 0 0 5" << endl;
    cout << "ObjectEnd # Axis" << endl;

    cout << "WorldBegin" << endl;
    cout << "AmbientLight 0.6 0.7 0.8 0.5" << endl;
    cout << "FarLight -1.0 0.0 -1.0 1.0 1.0 1.0 1.0" << endl;
    // cout << "PointLight 5 5 5 1 1 0 1" << endl;
    cout << "PointLight 25 25 50 1 1 0 1" << endl;
    cout << "PointLight 0 25 50 1 1 0 1" << endl;
    cout << "PointLight 25 0 50 1 1 0 1" << endl;
    // cout << "ObjectInstance \"Axis\"" << endl;
    cout << "PolySet \"PC\" " << coords.size() << " " << faceList.size()
         << endl;

    for (int i = 0; i < (int)coords.size(); ++i) {
        V3 color = interpolate(low_color, high_color,
                               scale_t_val(coords[i].z, lowest, highest));
        cout << coords[i].x << " " << coords[i].y << " " << coords[i].z << " "
             << color.x << " " << color.y << " " << color.z << endl;
    }

    for (int i = 0; i < (int)faceList.size(); ++i) {
        for (int j = 0; j < (int)faceList[i].size(); ++j) {
            cout << faceList[i][j] << " ";
        }
        cout << "-1" << endl;
    }

    // cout << "Translate 50 50 0" << endl;

    // int count = 0;
    // for (int i = 0; i < (int)sphere_coords.size(); ++i) {
    //     count += sphere_coords[i].size();
    // }

    // cout << "PolySet \"PC\" " << count << " " << sphere_faceList.size() <<
    // endl;

    // for (int i = 0; i < (int)sphere_coords.size(); ++i) {
    //     for (int j = 0; j < (int)sphere_coords[i].size(); ++j) {
    //         V3 color = interpolate(low_color, high_color,
    //                                scale_t_val(magnitude(sphere_coords[i][j]),
    //                                            lowest_sphere,
    //                                            highest_sphere));
    //         color = {.5, .5, .5};
    //         cout << sphere_coords[i][j].x << " " << sphere_coords[i][j].y <<
    //         " "
    //              << sphere_coords[i][j].z << " ";

    //         // cout << normals[i][j].x << " " << normals[i][j].y << " "
    //         //      << normals[i][j].z << " ";

    //         cout << color.x << " " << color.y << " " << color.z << endl;
    //     }
    // }

    // for (int i = 0; i < (int)sphere_faceList.size(); ++i) {
    //     for (int j = 0; j < (int)sphere_faceList[i].size(); ++j) {
    //         cout << sphere_faceList[i][j] << " ";
    //     }
    //     cout << "-1" << endl;
    // }

    cout << "WorldEnd" << endl;
}

// print out all of the values of a 2d vector
void print_vec(vector<vector<int>>& X) {
    for (int i = 0; i < (int)X.size(); ++i) {
        for (int j = 0; j < (int)X.size(); ++j) {
            cout << X[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// vector1 X vector2
V3 cross_product(V3 a, V3 b) {
    V3 ans;
    ans.x = a.y * b.z - a.z * b.y;
    ans.y = a.z * b.x - a.x * b.z;
    ans.z = a.x * b.y - a.y * b.x;
    // ans.x = a.y * b.z - a.z * b.y;
    // ans.y = -(a.x * b.z - a.z * b.x);
    // ans.z = a.x * b.y - a.y * b.x;
    return ans;
}

float magnitude(V3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

V3 normalize(V3 v) {
    float mag = magnitude(v);
    if (mag == 0) {
        return v;
    }
    return v / mag;
}

// used to create a t value for interpolate ie a value between 0-1
double scale_t_val(double value, double data_min, double data_max) {
    return (value - data_min) / (data_max - data_min);
}

V3 interpolate(V3 start, V3 end, double t) { return (end - start) * t + start; }

// V3 calculate_color() {}