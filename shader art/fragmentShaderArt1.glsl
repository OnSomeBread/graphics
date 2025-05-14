precision mediump float;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec3 palette(float t){
    vec3 a = vec3(.5);
    vec3 b = vec3(.5);
    vec3 c = vec3(1.);
    //vec3 d = vec3(.263, .416, .557);
    vec3 d = vec3(0.30, 0.20, 0.20);

    return a + b*cos( 6.283185*(c*t+d) );
}

float dot2( in vec2 v ) { return dot(v,v); }

float sdHeart( in vec2 p )
{
    p.x = abs(p.x);

    if( p.y+p.x>1.0 )
        return sqrt(dot2(p-vec2(0.25,0.75))) - sqrt(2.0)/4.0;
    return sqrt(min(dot2(p-vec2(0.00,1.00)),
                    dot2(p-0.5*max(p.x+p.y,0.0)))) * sign(p.x-p.y);
}

void main() {
    // gl_FragCoord.xy/u_resolution.xy;
    vec2 uv = (gl_FragCoord.xy * 2. - u_resolution.xy) / u_resolution.y;
    uv.y += .5;

    //vec3 col = palette(length(uv));
    //vec3 col = palette(sdHeart(uv));
    vec3 c1 = vec3(0.,1.,0.);
    vec3 c2 = vec3(0.1725, 0.7529, 0.098);
    float d = fract(sdHeart(uv) - u_time / 2.);
    vec3 col = 1. - vec3(smoothstep(.1, .3, d)) * c2;
    vec3 finalColor = abs(sin(col));

    gl_FragColor = vec4(col, 1.0);
}