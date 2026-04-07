#version 460 core

layout (location = 0) in uint vertex_data;

uniform ivec2 chunk_world_position;
uniform mat4 proj_view;

out vec2 TexCoord;

const vec2 vertex_uvs[4] = vec2[](
    vec2(0.0f / 16.0f, 1.0f / 16.0f),
    vec2(1.0f / 16.0f, 1.0f / 16.0f),
    vec2(1.0f / 16.0f, 0.0f / 16.0f),
    vec2(0.0f / 16.0f, 0.0f / 16.0f)
);

void main() {
    uint texture_index = vertex_data >> 17;
    int vertex_index = int(vertex_data & 0x1FFFF);

    int z = vertex_index % 17;
    int x = (vertex_index / 17) % 17;
    int y = vertex_index / (17 * 17);

    vec4 position = vec4(
        x + chunk_world_position.x,
        y,
        z + chunk_world_position.y,
        1.0
    );
    gl_Position = proj_view * position;

    const vec2 vertex_uv = vertex_uvs[gl_VertexID % 4];
    TexCoord = vec2(
        vertex_uv.x + float(texture_index % 16) / 16.0,
        vertex_uv.y + float(texture_index / 16) / 16.0
    );
}
