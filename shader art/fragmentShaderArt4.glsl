precision mediump float;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec2 u_mouse;


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
float sdHexPrism( vec3 p, vec2 h )
{
    vec3 q = abs(p);

    const vec3 k = vec3(-0.8660254, 0.5, 0.57735);
    p = abs(p);
    p.xy -= 2.0*min(dot(k.xy, p.xy), 0.0)*k.xy;
    vec2 d = vec2(
       length(p.xy - vec2(clamp(p.x, -k.z*h.x, k.z*h.x), h.x))*sign(p.y - h.x),
       p.z-h.y );
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float circle(vec3 p, float r) {
    return length(p) - r;
}

float density(vec3 p) {
    // Example of a moving noise-based density
    float d = sin(p.x * 1.5 + u_time) * cos(p.y * 1.2 - u_time * 0.5);
    d += sin(p.z * 2.0 + u_time * 0.3);
    return d * 0.2;
}

float map(vec3 p){
    p.z += u_time * .4;

    p.xy = fract(p.xy) - .5;
    p.z = mod(p.z, .25) - .125;

    float box = sdHexPrism(p, vec2(.05));

    return box;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
    vec2 m = (u_mouse * 2. - u_resolution.xy) / u_resolution.y;

    vec3 ro = vec3(0,0,-3.);
    vec3 rd = normalize(vec3(uv * 1.9, 1.));
    vec3 col = vec3(0.0, 0.0, 0.0);

    float t = 0.;

    // verticle rotation happens before the horizontal
    ro.yz *= rot2D(-m.y);
    rd.yz *= rot2D(-m.y);

    ro.xz *= rot2D(-m.x);
    rd.xz *= rot2D(-m.x);

    //m = vec2(cos(u_time*.2) , sin(u_time*.2));

    int j = 0;
    for(int i = 0; i < 80; i++){
        vec3 p = ro + rd * t;

        //p.xy *= rot2D(t * .2 * m.x);

        //p.y += sin(t*(m.y + 1.) * .5)*.35;

        float d = map(p);
        t += d;

        j = i;
        // close enough or too far away
        if(d < .001 || t > 500.) break;
    }

    col = palette(t * .09 + float(j) * .005);
    //col = vec3(t * .1);

    gl_FragColor = vec4(col, 1.);
}