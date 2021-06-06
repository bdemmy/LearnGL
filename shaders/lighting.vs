#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aB;
layout (location = 4) in float aC;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 bUV;
out vec3 bNormal;
out vec3 FragPos;

void main() {
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    bNormal = mat3(transpose(inverse(model))) * aNormal;

    // Unused
    bUV = aUV;
}