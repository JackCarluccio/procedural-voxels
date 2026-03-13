#include "graphics/renderer.h"

#include "graphics/graphics_core.h"
#include "graphics/element_buffer.h"
#include "graphics/shader_program.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"
#include "graphics/window.h"

#include <memory>

const float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};
const float indices[] = {
    0, 1, 2
};

std::unique_ptr<voxels::graphics::VertexBuffer> vertex_buffer;
std::unique_ptr<voxels::graphics::VertexArray> vertex_array;
std::unique_ptr<voxels::graphics::ElementBuffer> element_buffer;
std::unique_ptr<voxels::graphics::ShaderProgram> shader_program;

namespace voxels::graphics {

Renderer::Renderer(Window* window) : frame_(0), window_(window) {}

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

    shader_program->Use();
    vertex_buffer->Bind();
    vertex_array->Bind();
    element_buffer->Bind();
}

void Renderer::Render() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // double delta_time = (now - last_frame_time_).count();
    last_frame_time_ = now;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, element_buffer->GetCount());

    window_->SwapBuffers();

    frame_++;
}

} // namespace voxels::graphics
