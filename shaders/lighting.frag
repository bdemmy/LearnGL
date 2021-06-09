#version 330 core

in vec2 bUV;
in vec3 bNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

// Unused
uniform int textured;
uniform sampler2D tex1;
uniform sampler2D tex2;

vec3 norm;
vec3 lightDir;

layout (std140) uniform shader_data
{ 
    uniform mat4 view;
    uniform mat4 projection;
    uniform vec3 cameraPosition;
};

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
    vec3 viewDir = normalize(cameraPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor; 
    return specular;
}

void main() {
    vec3 baseColor = objectColor;
    if (textured != 0) {
        baseColor = vec3(texture(tex1, bUV));
    }
    norm = normalize(bNormal);
    lightDir = normalize(lightPos - FragPos);
    vec3 ambient = computeAmbient();
    vec3 diffuse = computeDiffuse();
    vec3 specular = computeSpecular();
    vec3 result = (ambient + diffuse + specular) * baseColor;
    FragColor = vec4(result, 1.0);
}