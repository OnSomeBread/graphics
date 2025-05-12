#version 460 core

layout(std430, binding=7) readonly buffer field_data_buffer {
    float field_data[];
};

out vec4 FragColor;

uniform float field_size;
uniform int field_rows;
uniform int field_cols;
uniform int field_planes;
uniform vec2 u_resolution;
uniform vec3 min_bound;
uniform vec3 max_bound;

uniform vec3 lightPos = vec3(-5., -5., 10);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.1804, 0.1804, 0.9216);
uniform float isoValue = .0;

// vec3 slow_particle_color = {.1, .25, 1};
// vec3 fast_particle_color = {.25, .55, .95};
// float fast_v = 4;  // highest value for color

// get the idx of the 3d space
int getIdx(ivec3 p) {
    return p.x * field_cols * field_planes + p.y * field_planes + p.z;
}

float trilinear_sample(vec3 world_pos) {
    vec3 voxel_pos = world_pos / field_size;
    ivec3 base = ivec3(floor(voxel_pos));
    vec3 f = fract(voxel_pos);

    float c000 = field_data[getIdx(base + ivec3(0, 0, 0))];
    float c100 = field_data[getIdx(base + ivec3(1, 0, 0))];
    float c010 = field_data[getIdx(base + ivec3(0, 1, 0))];
    float c110 = field_data[getIdx(base + ivec3(1, 1, 0))];
    float c001 = field_data[getIdx(base + ivec3(0, 0, 1))];
    float c101 = field_data[getIdx(base + ivec3(1, 0, 1))];
    float c011 = field_data[getIdx(base + ivec3(0, 1, 1))];
    float c111 = field_data[getIdx(base + ivec3(1, 1, 1))];

    float c00 = mix(c000, c100, f.x);
    float c01 = mix(c001, c101, f.x);
    float c10 = mix(c010, c110, f.x);
    float c11 = mix(c011, c111, f.x);

    float c0 = mix(c00, c10, f.y);
    float c1 = mix(c01, c11, f.y);

    return mix(c0, c1, f.z);
}

// void main(){
//     //vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
//     vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;

//     // vec3 ro = vec3(uv.x * 40.0, 0.0, uv.y * 40.0); // bottom of volume
//     // vec3 rd = vec3(0.0, 1.0, 0.0); // ray goes up
//     vec3 ro = vec3(uv.x * 40.0 + 40.0, 0.0, uv.y * 40.0 + 40.0); // center it in the volume
//     vec3 rd = vec3(0.0, 1.0, 0.0);

//     // vec3 ro = vec3(5.,1.,1.);
//     // vec3 rd = normalize(vec3(uv * 2., 1.)).xzy;

//     float t = 0.0;
//     float stepSize = 1.;
//     float total_density = 0.0;

//     for(int i = 0; i < 160; ++i){ // 160 steps for 80 units at 0.5 step
//         vec3 p = ro + rd * t;
//         if (any(lessThan(p, min_bound)) || any(greaterThanEqual(p, max_bound))) break;

//         //float d = field_data[getIdx(ivec3(int(p.x), int(p.y), int(p.z)))];
//         float d = trilinear_sample(p);
//         total_density += d * stepSize;
//         t += stepSize;
//     }

//     FragColor = vec4(vec3(total_density / 5.0), 1.0);
// }

// void main(){
//     vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;

//     vec3 ro = vec3(uv.x * 40.0 + 40.0, 0.0, uv.y * 40.0 + 40.0);
//     vec3 rd = vec3(0.0, 1.0, 0.0);

//     float t = 0.0;
//     float stepSize = .01;
//     float total_density = 0.0;

//     for (int i = 0; i < 160; ++i) {
//         vec3 p = ro + rd * t;
//         if (any(lessThan(p, min_bound)) || any(greaterThanEqual(p, max_bound))) break;

//         float d = trilinear_sample(p);
//         total_density += d * stepSize;
//         t += stepSize;
//     }

//     FragColor = vec4(vec3(total_density / 1.0), 1.0);
// }


void main() {
    vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;

    vec3 ro = vec3(40.0, -40.0, 20.0);
    // ray points to +p
    vec3 rd = normalize(vec3(uv * 1.0, 1.0)).xzy;

    vec3 inv_rd = 1.0 / rd;

    vec3 t0 = (min_bound - ro) * inv_rd;
    vec3 t1 = (max_bound - ro) * inv_rd;

    vec3 tmin3 = min(t0, t1);
    vec3 tmax3 = max(t0, t1);

    float t_enter = max(max(tmin3.x, tmin3.y), tmin3.z);
    float t_exit = min(min(tmax3.x, tmax3.y), tmax3.z);

    // Early exit if ray misses box
    if (t_exit < 0.0 || t_enter > t_exit) {
        FragColor = vec4(0.0);
        return;
    }

    float t = max(t_enter, 0.0);
    float stepSize = 1.0;
    float total_density = 0.0;

    for (int i = 0; i < 200; ++i) {
        if (t > t_exit) break;

        vec3 p = ro + rd * t;
        float d = trilinear_sample(p);
        total_density += d * stepSize;
        t += stepSize;
    }

    FragColor = vec4(vec3(total_density * 0.5), 1.0);
}