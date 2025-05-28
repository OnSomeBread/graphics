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

    vec3 ro = vec3(0,0,-3.);
    vec3 rd = normalize(vec3(uv * 1.9, 1.));
    vec3 col = vec3(0.0, 0.0, 0.0);
    
    // verticle rotation happens before the horizontal
    ro.yz *= rot2D(-m.y);
    rd.yz *= rot2D(-m.y);

    ro.xz *= rot2D(-m.x);
    rd.xz *= rot2D(-m.x);

    float t = rayMarch(ro, rd, 500.);
    vec3 skyColor = vec3(0.0196, 0.3529, 0.4863);
    // if(t > 500.) {
    //     gl_FragColor = vec4(skyColor + uv.y * .4, 1.);
    //     return;
    // }

    vec3 hit = ro + rd * t;
    

    vec3 normal = calc_normal(hit);
    vec3 lightSource = vec3(5., 5., -3.);
    vec3 lightDir = normalize(lightSource - hit);

    float diffuse = clamp(dot(lightDir, normal), 0., 1.);
    vec3 diffuseLight = diffuse * vec3(0.1294, 0.2706, 0.7333);// * vec3(exp(-t * .1));

    // vec3 halfVec = normalize(lightDir + normalize(rd));
    // float specular = clamp(dot(halfVec, normal), 0., 1.);
    // vec3 specularLight = pow(specular, 8.) * vec3(1.) * .1;

    vec3 reflectLight = normalize(reflect(-lightDir, normal));
    float specularStrength = clamp(dot(normalize(ro), reflectLight), 0., 1.) * float(dot(lightDir, normal) > 0.);
    vec3 specularLight = pow(specularStrength, 64.) * vec3(1.);

    col = diffuseLight + specularLight;

    // calculate shadows
    float distToLight = length(lightSource - hit);
    vec3 shadowRO = hit + normal * .1;
    float dist = rayMarch(shadowRO, lightDir, distToLight);

    // if raymarch hits something before max distance that means
    // shadow should apply here
    if(dist < distToLight) {
        col *= vec3(0.2196);
    }
    
    col = pow(col, vec3(1./2.2));

    gl_FragColor = vec4(col, 1.);
}