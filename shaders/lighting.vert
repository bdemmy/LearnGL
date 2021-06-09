#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in int aTextured;
layout (location = 4) in int aNormalized;

out vec2 bUV;
out vec3 bNormal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 normalModel;

layout (std140) uniform shader_data
{ 
    uniform mat4 view;
    uniform mat4 projection;
    uniform vec3 cameraPosition;
};

void main() {
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    bNormal = mat3(normalModel) * aNormal;

    // Unused
    bUV = aUV;
}