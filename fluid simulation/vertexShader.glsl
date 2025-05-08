#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aParticlePos;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragNormal;

void main(){
    vec3 worldPos = aPos + aParticlePos;
    gl_Position = projection * view * vec4(worldPos, 1.0);
    FragNormal = aNormal;
}