#include "application/application.h"

#include "graphics/graphics_core.h"
#include "graphics/window.h"
#include "graphics/renderer.h"

#include <stdexcept>

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

    renderer_ = std::make_unique<graphics::Renderer>(window_.get());
}

Application::~Application() {
    glfwTerminate();
}

void Application::Init() {
    renderer_->Init();
}

void Application::Run() {
    while (is_running_ && !window_->ShouldClose()) {
        Update();
        Draw();
    }
}

void Application::Update() {
    glfwPollEvents();
}

void Application::Draw() {
    renderer_->Render();
}

} // namespace voxels::application
