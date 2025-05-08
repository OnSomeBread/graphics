#include "marching_cubes.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::pair;
using std::string;
using std::to_string;
using std::vector;

// based on what vertex points are above the surfacelvl will return the related
// table index
int get_cubeIdx(vector<pair<V3, float>> &cube, float surfacelvl) {
    int cubeIdx = 0;
    for (int i = 0; i < 8; ++i) {
        if (cube[i].second < surfacelvl) {
            cubeIdx |= (1 << i);
        }
    }
    return cubeIdx;
}

// create a cube consisting of 8 V3s
vector<pair<V3, float>> create_cube(vector<vector<vector<float>>> &data, int r,
                                    int c, int p) {
    vector<vector<int>> cubeCoords = {
        {r, c, p},
        {r + 1, c, p},
        {r + 1, c, p + 1},
        {r, c, p + 1},
        {r, c + 1, p},
        {r + 1, c + 1, p},
        {r + 1, c + 1, p + 1},
        {r, c + 1, p + 1},
    };

    vector<pair<V3, float>> cube;
    for (auto c : cubeCoords) {
        V3 v;
        v.x = (float)c[0];
        v.y = (float)c[1];
        v.z = (float)c[2];
        cube.push_back({v, data[c[0]][c[1]][c[2]]});
    }

    return cube;
}

// gradient at the point i j k
V3 get_gradient(vector<vector<vector<float>>> &data, int i, int j, int k) {
    int nx = data.size();
    int ny = data[0].size();
    int nz = data[0][0].size();

    // the length of the approximation
    // making sure that the endpoints are only length 1
    float h1 = 2.0;
    float h2 = 2.0;
    float h3 = 2.0;
    if (i >= nx - 1 || i == 0) {
        h1 = 1;
    }
    if (j >= ny - 1 || j == 0) {
        h2 = 1;
    }
    if (k >= nz - 1 || k == 0) {
        h3 = 1;
    }

    V3 v;
    v.x = (data[min(i + 1, nx - 1)][j][k] - data[max(i - 1, 0)][j][k]) / h1;
    v.y = (data[i][min(j + 1, ny - 1)][k] - data[i][max(j - 1, 0)][k]) / h2;
    v.z = (data[i][j][min(k + 1, nz - 1)] - data[i][j][max(k - 1, 0)]) / h3;
    return v;
}

// get all of the edges that intersect with current cube
// since there are 12 edges there can only be at most 12 new V3s
vector<pair<V3, V3>> get_V3_coords(vector<vector<vector<float>>> &data,
                                   vector<pair<V3, float>> &cube, int cubeIdx,
                                   float surfacelvl) {
    vector<pair<V3, V3>> V3s(12);
    int edgeKey = verts_to_edges_table[cubeIdx];
    int idx = 0;
    while (edgeKey) {
        // if the first bit is 1
        if (edgeKey & 1) {
            // interpolate v1 and v2 at the surface level to get v3
            V3 v1 = cube[edges[idx][0]].first;
            V3 v2 = cube[edges[idx][1]].first;
            float t = (surfacelvl - cube[edges[idx][0]].second) /
                      (cube[edges[idx][1]].second - cube[edges[idx][0]].second);

            V3 g1 = get_gradient(data, v1.x, v1.y, v1.z);
            V3 g2 = get_gradient(data, v2.x, v2.y, v2.z);

            V3s[idx] = {interpolate(v2, v1, t), interpolate(g2, g1, t)};
        }
        ++idx;
        edgeKey >>= 1;
    }
    return V3s;
}

// using all of the interpolated V3s from the get_new_V3_coords func
// go through the triangulation table in sets of 3 and add them as a triangle to
// the triangles vec
void get_triangles(vector<vector<vector<float>>> &data,
                   vector<vector<pair<V3, V3>>> &triangles,
                   vector<pair<V3, float>> &cube, float surfacelvl) {
    int cubeIdx = get_cubeIdx(cube, surfacelvl);
    vector<pair<V3, V3>> V3s = get_V3_coords(data, cube, cubeIdx, surfacelvl);

    for (int i = 0; i < (int)triangulationTable[cubeIdx].size(); i += 3) {
        vector<pair<V3, V3>> triangle;

        // get each of the interpolated coords from V3s in sets of 3 that
        // make a triangle for the current cubeIdx
        triangle.push_back({V3s[triangulationTable[cubeIdx][i]].first,
                            V3s[triangulationTable[cubeIdx][i]].second});
        triangle.push_back({V3s[triangulationTable[cubeIdx][i + 1]].first,
                            V3s[triangulationTable[cubeIdx][i + 1]].second});
        triangle.push_back({V3s[triangulationTable[cubeIdx][i + 2]].first,
                            V3s[triangulationTable[cubeIdx][i + 2]].second});

        triangles.push_back(triangle);
    }
}

void print_marching_cubes_output(vector<V3> &coords, vector<V3> &normals,
                                 vector<vector<int>> &faceList) {
    cout << "PolySet \"PN\" " << coords.size() << " " << faceList.size()
         << endl;

    for (int i = 0; i < (int)coords.size(); ++i) {
        cout << coords[i].x << " " << coords[i].y << " " << coords[i].z << " "
             << normals[i].x << " " << normals[i].y << " " << normals[i].z
             << " ";
    }

    for (int i = 0; i < (int)faceList.size(); ++i) {
        for (int j = 0; j < (int)faceList[i].size(); ++j) {
            cout << faceList[i][j] << " ";
        }
        cout << "-1" << " ";
    }
}

// the main function that gets called by the water program
void marching_cubes(vector<vector<vector<float>>> &data, float surfacelvl) {
    vector<vector<pair<V3, V3>>> triangles;
    for (int r = 0; r < (int)data.size() - 1; ++r) {
        for (int c = 0; c < (int)data[r].size() - 1; ++c) {
            for (int p = 0; p < (int)data[r][c].size() - 1; ++p) {
                vector<pair<V3, float>> cube = create_cube(data, r, c, p);

                // add all of the triangles for this cubeIdx and surfacelvl to
                // the main triangles vec
                get_triangles(data, triangles, cube, surfacelvl);
            }
        }
    }

    vector<V3> coords;
    vector<V3> normals;
    vector<vector<int>> faceList;
    int vert = 0;
    for (int i = 0; i < (int)triangles.size(); ++i) {
        vector<int> face;
        for (int j = 0; j < (int)triangles[i].size(); ++j) {
            coords.push_back(triangles[i][j].first);
            normals.push_back(triangles[i][j].second * -1);
            face.push_back(vert);
            ++vert;
        }
        faceList.push_back(face);
    }

    print_marching_cubes_output(coords, normals, faceList);
}
