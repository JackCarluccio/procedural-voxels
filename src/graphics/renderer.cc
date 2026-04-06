#include "graphics/renderer.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

std::unique_ptr<voxels::graphics::ShaderProgram> shader_program;

namespace voxels::graphics {

void Renderer::Init() {
    shader_program = std::make_unique<graphics::ShaderProgram>(
        "assets/shaders/vertex_shader.vert",
        "assets/shaders/fragment_shader.frag"
    );
}

void Renderer::Draw(const Camera* camera, const std::unordered_map<glm::ivec2, std::unique_ptr<world::Chunk>, voxels::util::IVec2Hash>& chunks) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    shader_program->Use();
    shader_program->SetUniformMatrix4x4("view", glm::value_ptr(view));
    shader_program->SetUniformMatrix4x4("projection", glm::value_ptr(projection));

    for (const auto& [position, chunk] : chunks) {
        const std::unique_ptr<graphics::Mesh>& chunk_mesh = chunk->GetMesh();
        if (chunk_mesh == nullptr) {
            continue;
        }

        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                position.x * world::Chunk::SIZE,
                0.0f,
                position.y * world::Chunk::SIZE
            )
        );
        shader_program->SetUniformMatrix4x4("model", glm::value_ptr(model));

        chunk_mesh->Bind();
        glDrawElements(GL_TRIANGLES, chunk_mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
}

} // namespace voxels::graphics
