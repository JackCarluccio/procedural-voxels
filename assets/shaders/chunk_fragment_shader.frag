#version 460 core

in vec3 TexCoord;

out vec4 FragColor;

uniform sampler2DArray texture_atlas;

void main() {
    FragColor = texture(texture_atlas, TexCoord);
}
