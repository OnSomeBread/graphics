precision mediump float;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec2 u_mouse;

void main(){
    vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / min(u_resolution.x, u_resolution.y);

    for(float i = 1.0; i < 10.0; i++){
        uv.x += 0.6 / i * cos(i * 2.5* uv.y + u_time);
        uv.y += 0.6 / i * cos(i * 1.5 * uv.x + u_time);
    }
    
    gl_FragColor = vec4(vec3(0.1)/abs(sin(u_time-uv.y-uv.x)),1.0);
}