precision mediump float;

uniform vec2 u_resolution;
uniform float u_time;

vec3 palette(float t){
    vec3 a = vec3(.5);
    vec3 b = vec3(.5);
    vec3 c = vec3(1.);
    //vec3 d = vec3(.263, .416, .557);
    vec3 d = vec3(0.30, 0.20, 0.20);

    return a + b*cos( 6.283185*(c*t+d) );
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy * 2.0 - 1.0;
    uv.x *= u_resolution.x / u_resolution.y;

    vec3 finalColor = vec3(0.);

    vec2 uv0 = uv;
    for(float i = 0.; i < 6.; i += 1.){
        uv = fract(uv * 1.5) - .5;

        float d = length(uv) * exp(-length(uv0));

        vec3 col = palette(length(uv0) + i*.4 + u_time*.4);

        d = sin(d*8. + u_time * .8)/8.;
        d = pow(.01 / abs(d), 1.2);
        
        finalColor += (col * d) / 4.;
    }
    gl_FragColor = vec4(finalColor, 1.0);
}