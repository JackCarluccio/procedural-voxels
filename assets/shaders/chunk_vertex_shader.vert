#version 460 core

layout (location = 0) in uint vertex_data;

uniform ivec2 chunk_world_position;
uniform mat4 proj_view;

out vec2 TexCoord;

// const vec2 vertex_uvs[4] = vec2[](
//     vec2(0.0f / 16.0f, 1.0f / 16.0f),
//     vec2(1.0f / 16.0f, 1.0f / 16.0f),
//     vec2(1.0f / 16.0f, 0.0f / 16.0f),
//     vec2(0.0f / 16.0f, 0.0f / 16.0f)
// );

void main() {
    uint texture_index = vertex_data >> 19;
    int vertex_index = int(vertex_data & 0x3FFFF);

    int z = vertex_index & 0x1F; // % 32
    int x = (vertex_index >> 5) & 0x1F; // / 32 % 32
    int y = vertex_index >> 10; // / (32 * 32)

    vec4 position = vec4(
        x + chunk_world_position.x,
        y,
        z + chunk_world_position.y,
        1.0
    );
    gl_Position = proj_view * position;

    // Convert vertex_id into UV offset instead of indexing into a vertex_uvs array
    int vertex_id = gl_VertexID & 3;
    vec2 vertex_uv_offset = vec2(
        vertex_id == 1 || vertex_id == 2,
        vertex_id < 2
    );

    // Convert texture_index into UV coordinates
    vec2 vertex_uv = vec2(
        texture_index & 0xF, // % 16
        texture_index >> 4 // / 16
    );
    
    TexCoord = (vertex_uv + vertex_uv_offset) * (1.0f / 16.0f);
}
