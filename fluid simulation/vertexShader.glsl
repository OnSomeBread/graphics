#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aParticlePos;

uniform mat4 viewProjection;

out vec3 FragNormal;

void main(){
    vec3 worldPos = aPos + aParticlePos.xyz;
    gl_Position = viewProjection * vec4(worldPos, 1.0);
    FragNormal = aNormal;
}