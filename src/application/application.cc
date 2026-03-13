#include "application/application.h"

#include "graphics/graphics_core.h"
#include "graphics/element_buffer.h"
#include "graphics/shader_program.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"

#include <stdexcept>

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

namespace voxels::application {

Application::Application(int width, int height, const std::string& title)
    : is_running_(true)
{
    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Configure GLFW for OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = std::make_unique<graphics::Window>(width, height, title);
    window_->MakeContextCurrent();

    // Load OpenGL functions using GLAD
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        throw std::runtime_error("Failed to initialize OpenGL loader");
    }
}

Application::~Application() {
    glfwTerminate();
}

void Application::Init() {
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

void Application::Run() {
    shader_program->Use();
    vertex_buffer->Bind();
    vertex_array->Bind();
    element_buffer->Bind();

    while (is_running_ && !window_->ShouldClose()) {
        Update();
        Draw();
    }
}

void Application::Update() {
    glfwPollEvents();
}

void Application::Draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, element_buffer->GetCount());

    window_->SwapBuffers();
}

} // namespace voxels::application
