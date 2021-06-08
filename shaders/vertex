#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aB;
layout (location = 4) in float aC;

uniform mat4 model;

layout (std140) uniform shader_data
{ 
    uniform mat4 view;
    uniform mat4 projection;
};

out vec3 bColor;
out vec2 bUV;

void main() {
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);

    bUV = aUV;
}