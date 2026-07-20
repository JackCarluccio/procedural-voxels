#version 460 core

// Binary encoding format of vertex_data: (Ambient Occlusion, Texture Index, Y, X, Z)
// 0000 000A ATTT TTTT TYYY YYYY YYXX XXXZ ZZZZ
layout (location = 0) in uint vertex_data;

uniform ivec2 chunk_world_position;
uniform mat4 proj_view;

out vec3 TexCoord;
out float AmbientOcclusion;

float ambient_occlusion_lighting[4] = {
    1.0, // 0b00
    0.8, // 0b01
    0.6, // 0b10
    0.4  // 0b11
};

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
        (vertex_data >> 19) & 0xFF // Bits 19-26 encode texture index (0-255)
    );

    AmbientOcclusion = ambient_occlusion_lighting[(vertex_data >> 27) & 3];
}
