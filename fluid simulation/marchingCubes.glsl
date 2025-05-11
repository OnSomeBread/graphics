#version 460 core
layout(local_size_x = 2, local_size_y = 2, local_size_z = 2) in;

layout(std430, binding=3) buffer particles_buffer {
    vec4 particles[];
};

// layout(std430, binding=6){
//     vec3 nearby[];
// }

layout(std430, binding=7) buffer field_data_buffer {
    float field_data[];
};

layout(std430, binding=8) buffer triTable_buffer {
    int triTable[];
};

layout(std430, binding=9) buffer edges_buffer {
    int edges[];
};

layout(std430, binding=10) buffer edgeTable_buffer {
    int edgeTable[];
};

layout(std430, binding=11) buffer coords_buffer {
    vec4 coords[];
};

layout(std430, binding=12) buffer normals_buffer {
    vec4 normals[];
};

layout(std430, binding=13) buffer faceidx_buffer {
    uint faceIdx[];
};

uniform float surfacelvl;

// get the idx of the 3d space
int getIdx(int i, int j, int k) {
    return i * field_cols * field_planes + j * field_planes + k;
}

// gradient at the point i j k
vec3 get_gradient(int i, int j, int k) {
    int nx = field_rows;
    int ny = field_cols;
    int nz = field_planes;

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

    int index = i + j + k;

    vec3 v;
    v.x = (data[getIdx(min(i + 1, nx - 1), j, k)] - data[getIdx(max(i - 1, 0), j, k)]) / h1;
    v.y = (data[getidx(i, min(j + 1, ny - 1), k)] - data[getIdx(i, max(j - 1, 0), k)]) / h2;
    v.z = (data[getidx(i, j, min(k + 1, nz - 1))] - data[getIdx(i, j, max(k - 1, 0))]) / h3;

    return v;
}

// based on what vertex points are above the surfacelvl will return the related
// table index
int get_cubeIdx(vector<pair<vec3, float>> &cube) {
    int cubeIdx = 0;
    for (int i = 0; i < 8; ++i) {
        if (cube[i].second < surfacelvl) {
            cubeIdx |= (1 << i);
        }
    }
    return cubeIdx;
}

struct Cube {
    vec3 position;
    float value;
};

// create a cube consisting of 8 vec3s
void create_cube(int r, int c, int p, out Cube cube[8]) {
    ivec3 precube[8] = ivec3[8](
        ivec3(r, c, p),
        ivec3(r + 1, c, p),
        ivec3(r + 1, c, p + 1),
        ivec3(r, c, p + 1),
        ivec3(r, c + 1, p),
        ivec3(r + 1, c + 1, p),
        ivec3(r + 1, c + 1, p + 1),
        ivec3(r, c + 1, p + 1)
    );

    for (int i = 0; i < 8; ++i) {
        float val = field_data[getIdx(precube[i].x, precube[i].y, precube[i].z)];
        cube[i] = CubeVertex(pos, val);
    }
}

// get all of the edges that intersect with current cube
// since there are 12 edges there can only be at most 12 new vec3s
void get_vec3_coords(in Cube cube[8], int cubeIdx) {
    vector<pair<vec3, vec3>> vec3s(12);
    int edgeKey = verts_to_edges_table[cubeIdx];
    int idx = 0;
    while (edgeKey) {
        // if the first bit is 1
        if (edgeKey & 1) {
            // interpolate v1 and v2 at the surface level to get v3
            vec3 v1 = cube[edges[idx][0]].first;
            vec3 v2 = cube[edges[idx][1]].first;
            float t = (surfacelvl - cube[edges[idx][0]].second) /
                      (cube[edges[idx][1]].second - cube[edges[idx][0]].second);

            vec3 g1 = get_gradient(v1.x, v1.y, v1.z);
            vec3 g2 = get_gradient(v2.x, v2.y, v2.z);

            vec3s[idx] = {mix(v2, v1, t), mix(g2, g1, t)};
        }
        ++idx;
        edgeKey >>= 1;
    }
    return vec3s;
}

// using all of the interpolated vec3s from the get_new_vec3_coords func
// go through the triangulation table in sets of 3 and add them as a triangle to
// the triangles vec
void get_triangles(vector<vector<pair<vec3, vec3>>> &triangles, in Cube cube[8]) {
    int cubeIdx = get_cubeIdx(cube, surfacelvl);
    vector<pair<vec3, vec3>> vec3s = get_vec3_coords(cube, cubeIdx, surfacelvl);

    for (int i = 0; i < (int)triangulationTable[cubeIdx].size(); i += 3) {
        vector<pair<vec3, vec3>> triangle;

        // get each of the interpolated coords from vec3s in sets of 3 that
        // make a triangle for the current cubeIdx
        triangle.push_back({vec3s[triangulationTable[cubeIdx][i]].first,
                            vec3s[triangulationTable[cubeIdx][i]].second});
        triangle.push_back({vec3s[triangulationTable[cubeIdx][i + 1]].first,
                            vec3s[triangulationTable[cubeIdx][i + 1]].second});
        triangle.push_back({vec3s[triangulationTable[cubeIdx][i + 2]].first,
                            vec3s[triangulationTable[cubeIdx][i + 2]].second});

        triangles.push_back(triangle);
    }
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint z = gl_GlobalInvocationID.z;

    if (x >= field_rows || y >= field_cols || z >= field_planes) return;

    int i = int(x);
    int j = int(y);
    int k = int(z);

    int index = getIdx(i, j, k);

    Cube cube[8];
    create_cube(i, j, k, cube);

    // call get triangles
}