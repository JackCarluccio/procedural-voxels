#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in int texture_index;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    TexCoord = vec2(
        uv.x + float(texture_index % 16) / 16.0,
        uv.y + float(texture_index / 16) / 16.0
    );
}
