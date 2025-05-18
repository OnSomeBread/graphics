#version 460 core

layout(location = 2) in vec4 aParticlePos;

in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos = vec3(-5., -5., 10);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.0235, 0.2667, 0.5882);
uniform vec3 cameraPos;

// vec3 slow_particle_color = {.1, .25, 1};
// vec3 fast_particle_color = {.25, .55, .95};
// float fast_v = 4;  // highest value for color

void main(){
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(cameraPos - aParticlePos.xyz);

    // specular lighting also called blinn-phong lighting
    // adds a gloss glow onto a surface not realistic for water
    // but looks cool
    // vec3 halfvec = normalize(viewDir + lightDir);
    // vec3 specular = max(vec3(dot(halfvec, norm)), 0.);
    // vec3 specularLight = pow(specular, vec3(32.)) * int(diff > 0.);

    // fresnel glow effect 
    // FragColor = vec4(1-dot(norm, viewDir));
    // return ;

    // combine lighting
    vec3 result = diffuse * objectColor;
    FragColor = vec4(result, 1.0);
}