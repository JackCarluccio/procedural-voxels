#include "graphics/renderer.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"
#include "world/helper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <memory>

namespace {

unsigned int texture_atlas_id;
std::unique_ptr<voxels::graphics::ShaderProgram> shader_program;

}

namespace voxels::graphics {

void Renderer::Init() {
    shader_program = std::make_unique<graphics::ShaderProgram>(
        "assets/shaders/chunk_vertex_shader.vert",
        "assets/shaders/chunk_fragment_shader.frag"
    );

    constexpr int DEPTH = 256;
    constexpr int TILE_SIZE = 16;

    int width, height, channels;
    unsigned char* image_data = stbi_load("assets/textures/texture_atlas.png", &width, &height, &channels, 4);

    // Splice the atlas into a texture array
    int layer = 0;
    unsigned char* spliced_data = new unsigned char[width * height * 4];
    for (int atlas_y = 0; atlas_y < height; atlas_y += TILE_SIZE) {
        for (int atlas_x = 0; atlas_x < width; atlas_x += TILE_SIZE) {
            for (int i = 0; i < TILE_SIZE; i++) {
                unsigned char* src = image_data + ((atlas_y + i) * width + atlas_x) * 4;
                unsigned char* dst = spliced_data + (layer * TILE_SIZE + i) * TILE_SIZE * 4;
                std::memcpy(dst, src, TILE_SIZE * 4);
            }

            layer++;
        }
    }

    // Create texture array to prevent bleeding between tiles when using mipmaps
    GLuint texture_array_id;
    glGenTextures(1, &texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, TILE_SIZE, TILE_SIZE, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, spliced_data);

    // Generate mipmaps to prevent shimmering/sparkling on distant blocks
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Blend pixels when minified
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Keep pixels crisp when magnified

    stbi_image_free(image_data);
    delete[] spliced_data;
}

void Renderer::Draw(const Camera* camera, const std::unordered_map<glm::ivec2, std::unique_ptr<world::Chunk>, voxels::util::IVec2Hash>& chunks) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 proj_view = camera->GetProjectionMatrix() * camera->GetViewMatrix();

    shader_program->Use();
    shader_program->SetUniformMatrix4x4("proj_view", glm::value_ptr(proj_view));

    glBindTexture(GL_TEXTURE_2D, texture_atlas_id);

    for (const auto& [position, chunk] : chunks) {
        const std::unique_ptr<graphics::Mesh>& chunk_mesh = chunk->GetMesh();
        if (chunk_mesh == nullptr) {
            continue;
        }

        shader_program->SetUniform2i("chunk_world_position", position.x * world::CHUNK_SIZE, position.y * world::CHUNK_SIZE);

        chunk_mesh->Bind();
        glDrawElements(GL_TRIANGLES, chunk_mesh->GetIndexCount(), GL_UNSIGNED_SHORT, nullptr);
    }
}

} // namespace voxels::graphics
