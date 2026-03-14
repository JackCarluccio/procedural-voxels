#include "graphics/renderer.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"
#include "graphics/element_buffer.h"
#include "graphics/shader_program.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

const float vertices[] = {
    -0.5f, -0.5f, -2.0f,
     0.5f, -0.5f, -2.0f,
     0.0f,  0.5f, -2.0f
};
const float indices[] = {
    0, 1, 2
};

std::unique_ptr<voxels::graphics::VertexBuffer> vertex_buffer;
std::unique_ptr<voxels::graphics::VertexArray> vertex_array;
std::unique_ptr<voxels::graphics::ElementBuffer> element_buffer;
std::unique_ptr<voxels::graphics::ShaderProgram> shader_program;

namespace voxels::graphics {

Renderer::Renderer() : frame_(0) {}

void Renderer::Init() {
    vertex_buffer = std::make_unique<graphics::VertexBuffer>(sizeof(vertices), vertices, GL_STATIC_DRAW);
    vertex_array = std::make_unique<graphics::VertexArray>();
    vertex_array->LinkAttribute(*vertex_buffer, 0, 3, GL_FLOAT, 3 * sizeof(float), nullptr);
    vertex_array->EnableAttributeLayout(0);
    element_buffer = std::make_unique<graphics::ElementBuffer>(3, sizeof(indices), indices, GL_STATIC_DRAW);

    shader_program = std::make_unique<graphics::ShaderProgram>(
        "assets/shaders/vertex_shader.vert",
        "assets/shaders/fragment_shader.frag"
    );
}

void Renderer::Draw(const Camera* camera) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // double delta_time = (now - last_frame_time_).count();
    last_frame_time_ = now;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    vertex_buffer->Bind();
    vertex_array->Bind();
    element_buffer->Bind();

    shader_program->Use();
    shader_program->SetUniformMatrix4x4("model", glm::value_ptr(model));
    shader_program->SetUniformMatrix4x4("view", glm::value_ptr(view));
    shader_program->SetUniformMatrix4x4("projection", glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, element_buffer->GetCount());

    frame_++;
}

} // namespace voxels::graphics
