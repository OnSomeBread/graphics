precision mediump float;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec2 u_mouse;


float smin(float a, float b, float k){
    float h = max(k - abs(a - b), 0.) / k;
    return min(a, b) - h*h*h*k*(1./6.);
}

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

float circle(vec3 p, float r) {
    return length(p) - r;
}

float plane(vec3 p, vec3 n, float h) {
    return dot(p, n) + h;
}

float map(vec3 p) {
    float d1 = circle(p, 1.);
    float d2 = plane(p, normalize(vec3(0.,1.,0.)), 1.);
    return min(d1, d2);
}

vec3 calc_normal(vec3 p) {
    float h = .001;
    float dx = map(p + vec3(h, 0., 0.)) - map(p - vec3(h, 0., 0.));
    float dy = map(p + vec3(0., h, 0.)) - map(p - vec3(0., h, 0.));
    float dz = map(p + vec3(0., 0., h)) - map(p - vec3(0., 0., h));
    return normalize(vec3(dx, dy, dz) / (2. * h));
}

float rayMarch(vec3 ro, vec3 rd, float maxDist){
    float t = .0;
    for(int i = 0; i < 80; i++){
        vec3 p = ro + rd * t;

        float d = map(p);

        t += d;

        // close enough or too far away
        if(d < .001 || t > maxDist) break;
    }
    return t;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
    vec2 m = (u_mouse * 2. - u_resolution.xy) / u_resolution.y;

    vec2 col = abs(fract(uv * 10.) - .5);
    float dist = length(col - vec2(.5, .5));
    dist = smoothstep(dist, .0, .5);


    gl_FragColor = vec4(dist, dist, dist, 1.);
}