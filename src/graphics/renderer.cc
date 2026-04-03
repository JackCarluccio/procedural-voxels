#include "graphics/renderer.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

const float vertices[] = {
    -0.5f, -0.5f, -2.0f,
     0.5f, -0.5f, -2.0f,
     0.0f,  0.5f, -2.0f
};
const int indices[] = {
    0, 1, 2
};

std::unique_ptr<voxels::graphics::Mesh> mesh;
std::unique_ptr<voxels::graphics::ShaderProgram> shader_program;

namespace voxels::graphics {

void Renderer::Init() {
    mesh = std::make_unique<graphics::Mesh>(
        vertices, sizeof(vertices), indices, sizeof(indices), 3
    );
    mesh->LinkAttribute(0, 3, GL_FLOAT, 3 * sizeof(float), nullptr);

    shader_program = std::make_unique<graphics::ShaderProgram>(
        "assets/shaders/vertex_shader.vert",
        "assets/shaders/fragment_shader.frag"
    );
}

void Renderer::Draw(const Camera* camera) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    mesh->Bind();

    shader_program->Use();
    shader_program->SetUniformMatrix4x4("model", glm::value_ptr(model));
    shader_program->SetUniformMatrix4x4("view", glm::value_ptr(view));
    shader_program->SetUniformMatrix4x4("projection", glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, mesh->GetIndexCount());
}

} // namespace voxels::graphics
