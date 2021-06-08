#version 330 core

in vec2 bUV;

out vec4 FragColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    FragColor = mix(texture(tex1, bUV), texture(tex2, bUV), 0.15);
}