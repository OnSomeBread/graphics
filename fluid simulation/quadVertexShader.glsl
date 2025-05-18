#version 460 core
layout(location = 3) in vec2 aQuadPos;

layout(std430, binding=3) readonly buffer particles_buffer {
    vec4 particles[];
};

out vec2 quadPos;
out vec3 quadOffset;
out uint i;

uniform mat4 viewProjection;
uniform mat4 view;
uniform float size;

void main(){
    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 offset = (aQuadPos.x * camRight + aQuadPos.y * camUp) * size;
    vec3 worldPos = particles[gl_InstanceID].xyz + offset;

    i = gl_InstanceID;
    quadPos = aQuadPos;
    quadOffset = offset;
    gl_Position = viewProjection * vec4(worldPos, 1.0);
}