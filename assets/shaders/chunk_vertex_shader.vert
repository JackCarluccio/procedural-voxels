#version 460 core

layout (location = 0) in uint vertex_data;

uniform ivec2 chunk_world_position;
uniform mat4 proj_view;

out vec3 TexCoord;

void main() {
    int vertex_index = int(vertex_data & 0x3FFFF);

    // Use stride of 32 to encode x, z even though they're within [0, 16] for fast bitwise operations 
    int z = vertex_index & 0x1F;
    int x = (vertex_index >> 5) & 0x1F;
    int y = vertex_index >> 10;

    vec4 position = vec4(
        x + chunk_world_position.x,
        y,
        z + chunk_world_position.y,
        1.0
    );
    gl_Position = proj_view * position;

    // (0, 1), (1, 1), (1, 0), (0, 0)
    int vertex_id = gl_VertexID & 3;
    TexCoord = vec3(
        vertex_id == 1 || vertex_id == 2, // Boolean expression for u coordinate
        vertex_id < 2, // Boolean expression for v coordinate
        vertex_data >> 19 // Bits 19-26 encode texture index (0-255)
    );
}
