#version 460 core

layout(std430, binding=3) readonly buffer particles_buffer {
    vec4 particles[];
};

in vec2 quadPos;
in vec3 quadOffset;

in vec3 FragPos;
in flat uint i;
out vec4 FragColor;

uniform vec3 lightPos = vec3(125., 125., 250);
uniform vec3 farLightDir = normalize(vec3(-1, -1, 1));
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.0314, 0.349, 0.7608);
uniform vec3 cameraPos;
uniform float size;
uniform float u_time;

// vec3 slow_particle_color = {.1, .25, 1};
// vec3 fast_particle_color = {.25, .55, .95};
// float fast_v = 4;  // highest value for color

void main(){
    vec2 quadCenter = quadPos * 2. - 1.;
    float sqrDst = dot(quadCenter, quadCenter);
    if(sqrDst > 1) discard;

    // sphere imposter
    float quadz = sqrt(1 - sqrDst);
    vec3 quadNormal = normalize(vec3(quadCenter, quadz));

    // FragColor = vec4(quadNormal * 0.5 + 0.5, 1.);
    // return ;

    vec3 lightDir = normalize(lightPos - particles[i].xyz + quadOffset);
    vec3 viewDir = normalize(cameraPos - particles[i].xyz + quadOffset);

    float diffPointLight = clamp(dot(lightDir, quadNormal), 0., 1.);
    float diffFarLight = clamp(dot(farLightDir, quadNormal), 0., 1.);
    vec3 diffuse = diffPointLight * lightColor * objectColor;

    // fake water surface noise
    // quadNormal += 0.06 * sin(particles[i].xyz * 20.0); 
    // quadNormal = normalize(quadNormal);

    // float wave = sin(10.0 * quadCenter.x + u_time) * cos(10.0 * quadCenter.y + u_time);
    // quadNormal = normalize(quadNormal + vec3(0.1 * wave, 0.1 * wave, 0.0));

    // float fresnel = pow(1. - max(dot(quadNormal, -viewDir), 0.), 5.);
    // fresnel = mix(.02, 1., fresnel);

    // // both of these are meant to be sampled from the environment
    // // sky or other background color
    // vec3 reflectionColor = vec3(0.0, 0.0, 0.0);

    // // underwater and behind color  
    // vec3 refractionColor = vec3(0.0471, 0.4353, 0.6588);

    // vec3 col = mix(reflectionColor, refractionColor, fresnel);
    // FragColor = vec4(pow(col, vec3(1./2.2)), 1.);
    // return ;

    // specular light calculations
    vec3 halfvec = normalize(lightDir + viewDir);
    float specular = clamp(dot(halfvec, quadNormal), 0., 1.);
    vec3 specularLight = vec3(pow(specular, 8.)) * lightColor * .2;

    //FragColor = vec4(col + specularLight, 1.);

    // apply gamma correction to the final color
    vec3 finalColor = pow(diffuse + specularLight, vec3(1./2.2)) ;
    FragColor = vec4(finalColor, 1.);

    // float depth = length(particles[i].xyz - cameraPos);
    // FragColor = vec4(mix(vec4(0.), vec4(200.), depth / 200.));
}