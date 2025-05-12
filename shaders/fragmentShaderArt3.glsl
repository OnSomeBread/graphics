precision mediump float;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec2 u_mouse;

float sdSphere(vec3 p, float s){
    return length(p) - s;
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
}

float sdOctahedron(vec3 p, float s){
    p = abs(p);
    return (p.x + p.y + p.z - s) * .57735027;
}

float smin(float a, float b, float k){
    float h = max(k - abs(a - b), 0.) / k;
    return min(a, b) - h*h*h*k*(1./6.);
}

vec3 rot3D(vec3 p, vec3 axis, float angle) {
    return mix(dot(axis, p) * axis, p, cos(angle)) + cross(axis, p) * sin(angle);
}

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

vec3 palette(float t){
    vec3 a = vec3(.5);
    vec3 b = vec3(.5);
    vec3 c = vec3(1.);
    //vec3 d = vec3(.263, .416, .557);
    vec3 d = vec3(0.30, 0.20, 0.20);

    return a + b*cos( 6.283185*(c*t+d) );
}

float map(vec3 p){
    p.z += u_time * .4;

    p.xy = fract(p.xy) - .5;
    p.z = mod(p.z, .25) - .125;

    float box = sdOctahedron(p, .15);

    return box;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
    vec2 m = (u_mouse * 2. - u_resolution.xy) / u_resolution.y;

    vec3 ro = vec3(0,0,-3.);
    vec3 rd = normalize(vec3(uv * 1.9, 1.));
    vec3 col = vec3(0.);

    float t = 0.;

    // verticle rotation happens before the horizontal
    // ro.yz *= rot2D(-m.y);
    // rd.yz *= rot2D(-m.y);

    // ro.xz *= rot2D(-m.x);
    // rd.xz *= rot2D(-m.x);

    m = vec2(cos(u_time*.2) , sin(u_time*.2));

    const int iterations = 80;
    int j = 0;
    for(int i = 0; i < iterations; i++){
        vec3 p = ro + rd * t;

        p.xy *= rot2D(t * .2 * m.x);

        p.y += sin(t*(m.y + 1.) * .5)*.35;

        float d = map(p);
        t += d;

        j = i;
        // close enough or too far away
        if(d < .001 || t > 500.) break;
    }

    col = palette(t* .04 + float(j) * .005);

    gl_FragColor = vec4(col, 1.);
}