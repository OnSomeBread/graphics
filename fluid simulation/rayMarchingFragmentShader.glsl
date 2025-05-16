#version 460 core

layout(std430, binding=6) readonly buffer field_data_buffer {
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
uniform vec2 u_mouse;

uniform vec3 lightPos = vec3(-5., -5., 10);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.1804, 0.1804, 0.9216);

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

// calculates how much light is reflected and 1 - calc_reflectance is how much light is refracted
float calc_reflectance(vec3 inRay, vec3 normal, float airRefraction, float waterRefraction) {
    float refractRatio = airRefraction / waterRefraction;
    float cosIn = -dot(inRay, normal);
    float sinSqrRefract = refractRatio * refractRatio * (1 - cosIn * cosIn);
    if(sinSqrRefract >= 1) return 1;

    float cosRefract = sqrt(1 - sinSqrRefract);
    float sqrtRayPerp = (airRefraction * cosIn - waterRefraction * cosRefract) / (airRefraction * cosIn + waterRefraction * cosRefract);
    float sqrtRayParallel = (waterRefraction * cosIn - airRefraction * cosRefract) / (waterRefraction * cosIn + airRefraction * cosRefract);

    return (sqrtRayPerp * sqrtRayPerp + sqrtRayParallel * sqrtRayParallel) / 2.; 
}

// calculates direction of refraction given in coming direction
vec3 refract(vec3 inRay, vec3 normal, float airRefraction, float waterRefraction) {
    float refractRatio = airRefraction / waterRefraction;
    float cosIn = -dot(inRay, normal);
    float sinSqrRefract = refractRatio * refractRatio * (1 - cosIn * cosIn);
    return inRay * refractRatio + normal * (refractRatio * cosIn - sqrt(1 - sinSqrRefract));
}

// this works well for 3d rotations using swizzling 
// the axis missing from the swizzle is the axis of rotation
mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

// central differences normal approximation
vec3 calc_normal(vec3 p) {
    float h = .1;
    float dx = trilinear_sample(p + vec3(h, 0., 0.)) - trilinear_sample(p - vec3(h, 0., 0.));
    float dy = trilinear_sample(p + vec3(0., h, 0.)) - trilinear_sample(p - vec3(0., h, 0.));
    float dz = trilinear_sample(p + vec3(0., 0., h)) - trilinear_sample(p - vec3(0., 0., h));
    return normalize(vec3(dx, dy, dz) / (2. * h));
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
    vec2 m = (u_mouse * 2. - u_resolution.xy) / u_resolution.y;

    vec3 ro = vec3(40.0, -40.0, 20.0);
    // funky swizzle is to make ray point to +y
    vec3 rd = normalize(vec3(uv * 1.0, 1.0)).xzy;

    // verticle rotation happens before the horizontal
    ro.yz *= rot2D(-m.y);
    rd.yz *= rot2D(-m.y);

    ro.xz *= rot2D(-m.x);
    rd.xz *= rot2D(-m.x);

    vec3 inv_rd = 1.0 / rd;

    vec3 t0 = (min_bound - ro) * inv_rd;
    vec3 t1 = (max_bound - ro) * inv_rd;

    vec3 tmin3 = min(t0, t1);
    vec3 tmax3 = max(t0, t1);

    float t_enter = max(max(tmin3.x, tmin3.y), tmin3.z);
    float t_exit = min(min(tmax3.x, tmax3.y), tmax3.z);

    // early exit if ray misses box
    if (t_exit < 0.0 || t_enter > t_exit) {
        FragColor = vec4(0.0);
        return;
    }

    float t = max(t_enter, 0.0);
    float stepSize = 0.01;
    float total_density = 0.0;
    vec3 total_light = vec3(0.);

    vec3 scatteringCoefficients = vec3(0.2588, 0.6039, 0.7412);
    float speedVacuum = 299792458;
    float speedLight = 299702547;
    float speedWater = 225000000;

    float airRefraction = speedVacuum / speedLight;
    float waterRefraction = speedVacuum / speedWater;

    for (int i = 0; i < 100. / stepSize; ++i) {
        if (t > t_exit) break;

        vec3 p = ro + rd * t;
        float d = trilinear_sample(p) * stepSize;

        total_density += d;

        //float sunRayColor = 

        vec3 scatteredLight = vec3(1.) * d * scatteringCoefficients;
        vec3 rayTransmittance = exp(-d * scatteringCoefficients);
        total_light += scatteredLight * rayTransmittance;
        t += stepSize;
    }

    FragColor = vec4(total_light, 1.0);
    //FragColor = vec4(vec3(total_density * 0.5), 1.0);
}