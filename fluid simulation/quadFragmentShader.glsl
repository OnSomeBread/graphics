#version 460 core

layout(std430, binding=3) readonly buffer particles_buffer {
    vec4 particles[];
};

in vec2 quadPos;
in vec3 FragPos;
in flat uint i;
out vec4 FragColor;

uniform vec3 lightPos = vec3(-5., -5., 10);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.0275, 0.2784, 0.6078);
uniform vec3 cameraPos = vec3(15., -40., 35.);

// vec3 slow_particle_color = {.1, .25, 1};
// vec3 fast_particle_color = {.25, .55, .95};
// float fast_v = 4;  // highest value for color

void main(){
    vec2 quadCenter = quadPos * 2. - 1.;
    float sqrDst = dot(quadCenter, quadCenter);
    if(sqrDst > 1) discard;

    float depth = length(particles[i].xyz - cameraPos);
    //FragColor = vec4(depth);
    FragColor = vec4(0.1373, 0.5255, 0.7059, 1.0);
}