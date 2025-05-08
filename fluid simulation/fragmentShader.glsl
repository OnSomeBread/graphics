#version 330 core

in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos = vec3(10.0, 10.0, 10.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.1804, 0.1804, 0.9216);

void main(){
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = diffuse * objectColor;
    FragColor = vec4(result, 1.0);
}