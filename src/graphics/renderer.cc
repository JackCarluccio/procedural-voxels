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

    int width, height, channels;
    unsigned char* data = stbi_load("assets/textures/texture_atlas.png", &width, &height, &channels, 0);
    glGenTextures(1, &texture_atlas_id);
    glBindTexture(GL_TEXTURE_2D, texture_atlas_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(data);
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
        glDrawElements(GL_TRIANGLES, chunk_mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
}

} // namespace voxels::graphics
