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

uniform float density_mult = 1.;
uniform float isovalue = .01;

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

float trilinear_density_sample(vec3 world_pos) {
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

    return mix(c0, c1, f.z) * density_mult;
}

vec3 waterRefract(vec3 inDir, vec3 normal, float airRefraction, float waterRefraction){
    float refractRatio = airRefraction / waterRefraction;
    float cosAngleIn = -dot(inDir, normal);
    float sinSqrAngleOfRefraction = refractRatio * refractRatio * (1 - cosAngleIn * cosAngleIn);

    if (sinSqrAngleOfRefraction > 1.) return vec3(0.);

    vec3 refractDir = refractRatio * inDir + (refractRatio * cosAngleIn - sqrt(1. - sinSqrAngleOfRefraction)) * normal;
    return refractDir;
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
    float dx = trilinear_density_sample(p + vec3(h, 0., 0.)) - trilinear_density_sample(p - vec3(h, 0., 0.));
    float dy = trilinear_density_sample(p + vec3(0., h, 0.)) - trilinear_density_sample(p - vec3(0., h, 0.));
    float dz = trilinear_density_sample(p + vec3(0., 0., h)) - trilinear_density_sample(p - vec3(0., 0., h));
    return normalize(vec3(dx, dy, dz));
}

// find at what t value the ray enters and exits the simulation bounds
vec2 calc_ray_bounds(vec3 ro, vec3 rd) {
    // vec3 inv_rd = vec3(
    //     rd.x != 0.0 ? 1.0 / rd.x : (rd.x >= 0.0 ? 1e10 : -1e10),
    //     rd.y != 0.0 ? 1.0 / rd.y : (rd.y >= 0.0 ? 1e10 : -1e10),
    //     rd.z != 0.0 ? 1.0 / rd.z : (rd.z >= 0.0 ? 1e10 : -1e10)
    // );

    vec3 t0 = (min_bound - ro) / rd;
    vec3 t1 = (max_bound - ro) / rd;

    vec3 tmin3 = min(t0, t1);
    vec3 tmax3 = max(t0, t1);

    float t_enter = max(max(tmin3.x, tmin3.y), tmin3.z);
    float t_exit = min(min(tmax3.x, tmax3.y), tmax3.z);

    // distance to the box and distance inside of box
    return vec2(max(0, t_enter), max(0, t_exit - max(0, t_enter)));

    return vec2(t_enter, t_exit);
}

// marches through a ray direction and samples the density along the ray at stepSize intervals
float total_density_on_ray(vec3 ro, vec3 rd, float dist, float stepSize){
    float total_density = 0.;
    for(float t = 0.; t < dist; t += stepSize){
        vec3 p = ro + rd * t;
        total_density += trilinear_density_sample(p) * stepSize;
    }
    return total_density;
}

float total_transmittance_along_ray(vec3 ro, vec3 rd, float dist, float stepSize) {
    float transmittance = 1.;
    for(float t = 0.; t < dist; t += stepSize){
        vec3 p = ro + rd * t;

        float d = trilinear_density_sample(p);
        transmittance *= exp(-d * stepSize);

        if (transmittance < 0.001) {
            break;
        }
    }
    return transmittance;
}

// gets the exact position of water from some isovalue in the density field
float find_water_surface(vec3 ro, vec3 rd, float stepSize) {
    vec2 t_bounds = calc_ray_bounds(ro, rd);

    // early exit if ray misses box
    if (t_bounds.y < 0. || t_bounds.x >= t_bounds.y) {
        FragColor = vec4(0.);
        return -1;
    }

    float t = max(t_bounds.x, 0.);
    float prev_density = trilinear_density_sample(ro + rd * t);

    for (; t < t_bounds.y; t += stepSize) {
        vec3 p = ro + rd * t;
        float curr_density = trilinear_density_sample(p);

        if (prev_density < isovalue && curr_density >= isovalue) {
            float alpha = (isovalue - prev_density) / (curr_density - prev_density);
            t = t - stepSize + alpha * stepSize;

            break;
        }

        prev_density = curr_density;
    }

    return t;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
    vec2 m = (u_mouse * 2. - u_resolution.xy) / u_resolution.y;

    vec3 ro = vec3(75.0, -75.0, 40.0);
    // funky swizzle is to make ray point to +y
    vec3 rd = normalize(vec3(uv * 1.0, 1.0)).xzy;
    vec3 lightDir = normalize(vec3(-2., -2., .5));

    // verticle rotation happens before the horizontal
    // ro.yz *= rot2D(-m.y);
    // rd.yz *= rot2D(-m.y);

    // ro.xz *= rot2D(-m.x);
    // rd.xz *= rot2D(-m.x);

    float speedVacuum = 299792458;
    float speedLight = 299702547;
    float speedWater = 225000000;

    float airRefraction = speedVacuum / speedLight;
    float waterRefraction = speedVacuum / speedWater;

    // for(; t < t_bounds.y; t += stepSize){
    //     vec3 p = ro + rd * t;
    //     float d = trilinear_density_sample(p) * stepSize;

    //     total_density += d;
    //     if(total_density > isovalue){
    //         break;
    //     }
    // }

    vec2 t_bounds = calc_ray_bounds(ro, rd);

    // early exit if ray misses box
    if (t_bounds.y < 0. || t_bounds.x >= t_bounds.y) {
        FragColor = vec4(0.);
        return;
    }

    float transmittance = 1.;
    float transmittance_coeff = 1.;
    float light = 0.;
    vec3 hit;

    for(int i = 0; i < 1; ++i) {
        float t = find_water_surface(ro, rd, .1);
        if(t == -1) break;

        hit = ro + rd * t;
        vec3 normal = calc_normal(hit);

        vec3 reflectDir = normalize(reflect(rd, normal));
        vec3 refractDir = normalize(waterRefract(rd, normal, airRefraction, waterRefraction));
        float reflectance = calc_reflectance(rd, normal, airRefraction, waterRefraction);

        // sample densities along both ray directions
        float density_reflect = total_density_on_ray(hit + reflectDir * .1, reflectDir, 100., .5);
        float density_refract = total_density_on_ray(hit + refractDir * .1, refractDir, 100., .5);
        
        bool goRefractDir = density_reflect * reflectance < density_refract * (1. - reflectance);
        if(goRefractDir) {
            light += transmittance * exp(-density_reflect * transmittance_coeff) * reflectance;
        }
        else {
            light += transmittance * exp(-density_refract * transmittance_coeff) * (1. - reflectance);
        }

        rd = goRefractDir ? refractDir : reflectDir;
        transmittance *= goRefractDir ? (1. - reflectance) : reflectance;
    }

    light += transmittance * exp(-total_density_on_ray(hit, rd, 100., .5) * transmittance_coeff);
    
    // FragColor = vec4(density_refract * (1 - reflectance), 0., density_reflect * reflectance, 1.);
    // return ;
    //vec3 waterCol = mix(transmittance_refract * vec3(0.0784, 0.2353, 0.5804), transmittance_reflect * vec3(1.0, 1.0, 1.0), vec3(reflectance));

    FragColor = vec4(pow((1. - light) * vec3(1.0, 1.0, 1.0), vec3(1./2.2)), 1.);
}