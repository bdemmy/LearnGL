#version 330 core

in vec2 bUV;
in vec3 bNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

// Unused
uniform sampler2D tex1;
uniform sampler2D tex2;

vec3 computeAmbient() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    return ambient * objectColor;
}

vec3 computeDiffuse() {
    vec3 norm = normalize(bNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    return diff * lightColor;
}

void main() {
    vec3 ambient = computeAmbient();
    vec3 diffuse = computeDiffuse();
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}