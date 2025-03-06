#include "proj2.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::string;
using std::to_string;
using std::vector;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "usage ./proj2 input_file output_file surfacelvl" << endl;
        return 0;
    }
    string filename = argv[1];

    std::ifstream file(filename, std::ios::binary);

    // grab first few values
    int cols, rows, planes;
    file >> cols >> rows >> planes;
    cout << "cols:" << cols << " rows:" << rows << " planes:" << planes << endl;

    // clear the whitespace
    char whitespace;
    file.read(&whitespace, 1);

    // 3d array to hold all of the data
    vector<vector<vector<float>>> data(
        planes, vector<vector<float>>(rows, vector<float>(cols)));

    // read each value assuming little endian binary floats
    float value;
    for (int p = 0; p < planes; ++p) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                file.read(reinterpret_cast<char *>(&value), sizeof(float));
                data[p][r][c] = value;
            }
        }
    }

    file.close();

    // for (int p = 0; p < planes; ++p) {
    //     cout << "plane " << p << ":\n";
    //     for (int r = 0; r < rows; ++r) {
    //         for (int c = 0; c < cols; ++c) {
    //             cout << data[p][r][c] << " ";
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    // }

    float surfacelvl = std::stof(argv[3]);
    vector<vector<Voxel>> triangles;
    for (int p = 0; p < planes - 1; ++p) {
        for (int r = 0; r < rows - 1; ++r) {
            for (int c = 0; c < cols - 1; ++c) {
                vector<Voxel> cube = create_cube(data, p, r, c);

                // add all of the triangles for this cubeIdx and surfacelvl to
                // the main triangles vec
                get_triangles(data, triangles, cube, surfacelvl);
            }
        }
    }

    // turn triangles into a polyset of coords and faces
    // Voxels here won't have value init
    vector<Voxel> coords;
    vector<vector<int>> faceList;
    int vert = 0;
    for (int i = 0; i < (int)triangles.size(); ++i) {
        vector<int> face;
        for (int j = 0; j < (int)triangles[i].size(); ++j) {
            // cout << triangles[i][j].x << " " << triangles[i][j].y << " "
            //      << triangles[i][j].z << endl;
            coords.push_back(triangles[i][j]);
            face.push_back(vert);
            ++vert;
        }
        faceList.push_back(face);
    }

    // finally create the polyset file
    writeOutput(argv[2], coords, faceList);
}

// based on what vertex points are above the surfacelvl will return the related
// table index
int get_cubeIdx(vector<Voxel> &cube, float surfacelvl) {
    int cubeIdx = 0;
    for (int i = 0; i < 8; ++i) {
        if (cube[i].value < surfacelvl) {
            cubeIdx |= (1 << i);
        }
    }
    return cubeIdx;
}

// create a cube consisting of 8 voxels
vector<Voxel> create_cube(vector<vector<vector<float>>> &data, int p, int r,
                          int c) {
    vector<vector<int>> cubeCoords = {
        {p, r, c},
        {p + 1, r, c},
        {p + 1, r, c + 1},
        {p, r, c + 1},
        {p, r + 1, c},
        {p + 1, r + 1, c},
        {p + 1, r + 1, c + 1},
        {p, r + 1, c + 1},
    };

    vector<Voxel> cube;
    for (auto c : cubeCoords) {
        // cubeValues.push_back(p, r, c, data[p][r][c])
        Voxel v;
        v.x = (float)c[0];
        v.y = (float)c[1];
        v.z = (float)c[2];
        v.value = data[c[0]][c[1]][c[2]];
        cube.push_back(v);
    }

    return cube;
}

// gradient at the point i j k
Voxel get_gradient(vector<vector<vector<float>>> &data, int i, int j, int k) {
    int nx = data.size();
    int ny = data[0].size();
    int nz = data[0][0].size();

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

    Voxel v;
    v.x = (data[min(i + 1, nx - 1)][j][k] - data[max(i - 1, 0)][j][k]) / h1;
    v.y = (data[i][min(j + 1, ny - 1)][k] - data[i][max(j - 1, 0)][k]) / h2;
    v.z = (data[i][j][min(k + 1, nz - 1)] - data[i][j][max(k - 1, 0)]) / h3;
    return v;
}

// interpolate where the voxel should be given 2 voxels and the expected
// surfacelvl
Voxel interpolate(Voxel v1, Voxel v2, Voxel g1, Voxel g2, float surfacelvl) {
    // interpolation factor a guess on where the point
    // should be based of v1 and v2 values
    double t = (surfacelvl - v1.value) / (v2.value - v1.value);

    // calculate where the v3 should be based off the interpolation guess
    Voxel v3;
    v3.x = t * (v2.x - v1.x) + v1.x;
    v3.y = t * (v2.y - v1.y) + v1.y;
    v3.z = t * (v2.z - v1.z) + v1.z;

    v3.nx = t * (g2.x - g1.x) + g1.x;
    v3.ny = t * (g2.y - g1.y) + g1.y;
    v3.nz = t * (g2.z - g1.z) + g1.z;

    return v3;
}

// get all of the edges that intersect with current cube
// since there are 12 edges there can only be at most 12 new voxels
vector<Voxel> get_new_voxel_coords(vector<vector<vector<float>>> &data,
                                   vector<Voxel> &cube, int cubeIdx,
                                   float surfacelvl) {
    vector<Voxel> voxels(12);
    int edgeKey = verts_to_edges_table[cubeIdx];
    int idx = 0;
    while (edgeKey) {
        // if the first bit is 1
        if (edgeKey & 1) {
            // interpolate v1 and v2 at the surface level to get v3
            Voxel v1 = cube[edges[idx][0]];
            Voxel v2 = cube[edges[idx][1]];

            Voxel g1 = get_gradient(data, v1.x, v1.y, v1.z);
            g1.value = v1.value;
            Voxel g2 = get_gradient(data, v2.x, v2.y, v2.z);
            g2.value = v2.value;

            voxels[idx] = interpolate(v1, v2, g1, g2, surfacelvl);
        }
        ++idx;
        // right shift 1
        edgeKey >>= 1;
    }
    return voxels;
}

// using all of the interpolated voxels from the get_new_voxel_coords func
// go through the triangulation table in sets of 3 and add them as a triangle to
// the triangles vec
void get_triangles(vector<vector<vector<float>>> &data,
                   vector<vector<Voxel>> &triangles, vector<Voxel> &cube,
                   float surfacelvl) {
    int cubeIdx = get_cubeIdx(cube, surfacelvl);
    vector<Voxel> voxels =
        get_new_voxel_coords(data, cube, cubeIdx, surfacelvl);

    for (int i = 0; i < (int)triangulationTable[cubeIdx].size(); i += 3) {
        vector<Voxel> triangle;

        // get each of the interpolated coords from voxels in sets of 3 that
        // make a triangle for the current cubeIdx
        triangle.push_back(voxels[triangulationTable[cubeIdx][i]]);
        triangle.push_back(voxels[triangulationTable[cubeIdx][i + 1]]);
        triangle.push_back(voxels[triangulationTable[cubeIdx][i + 2]]);

        triangles.push_back(triangle);
    }
}

// turns lines into lines of a new file
void writeOutput(string fileName, vector<Voxel> &coords,
                 vector<vector<int>> &facesList) {
    // write all changes to a new file starting with the header
    string output = "PolySet \"PN\" " + to_string(coords.size()) + " " +
                    to_string(facesList.size()) + "\n";
    for (int i = 0; i < (int)coords.size(); ++i) {
        output += to_string(coords[i].x) + " " + to_string(coords[i].y) + " " +
                  to_string(coords[i].z) + " ";
        output += to_string(coords[i].nx) + " " + to_string(coords[i].ny) +
                  " " + to_string(coords[i].nz) + "\n";
    }

    // now add all of the faces to the output file
    output += "\n";
    for (int i = 0; i < (int)facesList.size(); i++) {
        for (int j = 0; j < (int)facesList[i].size(); j++) {
            output += to_string(facesList[i][j]) + " ";
        }
        output += "-1\n";
    }

    // write to output file
    std::ofstream outputFile(fileName);
    outputFile << output;
    outputFile.close();
}