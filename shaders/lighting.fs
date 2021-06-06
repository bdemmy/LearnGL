#version 330 core

in vec2 bUV;
in vec3 bNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

// Unused
uniform sampler2D tex1;
uniform sampler2D tex2;

vec3 norm;
vec3 lightDir;

vec3 computeAmbient() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    return ambient * objectColor;
}

vec3 computeDiffuse() {
    float diff = max(dot(norm, lightDir), 0.0);
    return diff * lightColor;
}

vec3 computeSpecular() {
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor; 
    return specular;
}

void main() {
    norm = normalize(bNormal);
    lightDir = normalize(lightPos - FragPos);
    //vec3 oColor = vec3(mix(texture(tex1, bUV), texture(tex2, bUV), .2));
    vec3 ambient = computeAmbient();
    vec3 diffuse = computeDiffuse();
    vec3 specular = computeSpecular();
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}