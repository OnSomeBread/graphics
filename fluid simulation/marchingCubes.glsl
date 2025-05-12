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

layout(binding = 2) uniform atomic_uint triangleCounter;

uniform float surfacelvl;
uniform int field_rows;
uniform int field_cols;
uniform int field_planes;

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
    v.x = (field_data[getIdx(min(i + 1, nx - 1), j, k)] - field_data[getIdx(max(i - 1, 0), j, k)]) / h1;
    v.y = (field_data[getIdx(i, min(j + 1, ny - 1), k)] - field_data[getIdx(i, max(j - 1, 0), k)]) / h2;
    v.z = (field_data[getIdx(i, j, min(k + 1, nz - 1))] - field_data[getIdx(i, j, max(k - 1, 0))]) / h3;

    return v;
}

struct Cube {
    vec3 position;
    float value;
};

// based on what vertex points are above the surfacelvl will return the related
// table index
int get_cubeIdx(in Cube cube[8]) {
    int cubeIdx = 0;
    for (int i = 0; i < 8; ++i) {
        if (cube[i].value < surfacelvl) {
            cubeIdx |= (1 << i);
        }
    }
    return cubeIdx;
}

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
        cube[i] = Cube(vec3(precube[i].x, precube[i].y, precube[i].z), val);
    }
}

// get all of the edges that intersect with current cube
// since there are 12 edges there can only be at most 12 new vec3s
void get_vec3_coords(in Cube cube[8], int cubeIdx, out vec3 tricoords[12], out vec3 trinormals[12]) {
    int edgeKey = edgeTable[cubeIdx];
    int idx = 0;
    while (edgeKey > 0) {
        // if the first bit is 1
        if ((edgeKey & 1) != 0) {
            // interpolate v1 and v2 at the surface level to get v3
            vec3 v1 = cube[edges[idx * 2]].position;
            vec3 v2 = cube[edges[idx * 2 + 1]].position;
            float t = (surfacelvl - cube[edges[idx * 2]].value) /
                      (cube[edges[idx * 2 + 1]].value - cube[edges[idx * 2]].value);

            vec3 g1 = get_gradient(int(v1.x), int(v1.y), int(v1.z));
            vec3 g2 = get_gradient(int(v2.x), int(v2.y), int(v2.z));
            tricoords[idx] = mix(v2, v1, t);
            trinormals[idx] = mix(g2, g1, t);
        }
        ++idx;
        edgeKey >>= 1;
    }
}

// using all of the interpolated vec3s from the get_new_vec3_coords func
// go through the triangulation table in sets of 3 and add them as a triangle to
// the triangles vec
void get_triangles(in Cube cube[8]) {
    int cubeIdx = get_cubeIdx(cube);
    vec3 tricoords[12];
    vec3 trinormals[12];
    get_vec3_coords(cube, cubeIdx, tricoords, trinormals);

    for (int i = 0; i < 16 && triTable[cubeIdx * 16 + i] != -1; i += 3) {
        for (int j = 0; j < 3; ++j) {
            int edgeIdx = triTable[cubeIdx * 16 + i];
            vec3 position = tricoords[edgeIdx];
            vec3 normal = trinormals[edgeIdx];

            uint idx = atomicCounterIncrement(triangleCounter);
            coords[idx] = vec4(position, 1.0);
            normals[idx] = vec4(normalize(normal), 0.0);
            faceIdx[idx] = idx;
        }
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