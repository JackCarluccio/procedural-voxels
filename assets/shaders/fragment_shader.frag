#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_atlas;

void main() {
    FragColor = texture(texture_atlas, TexCoord);
}
