#include "application/application.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"
#include "graphics/renderer.h"
#include "graphics/window.h"
#include "input/input_manager.h"
#include "world/chunk_manager.h"

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

    camera_ = std::make_unique<graphics::Camera>(
        glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        100.0f
    );

    renderer_ = std::make_unique<graphics::Renderer>();
    input_manager_ = std::make_unique<input::InputManager>(window_->GetGLFWwindow(), *camera_);
    chunk_manager_ = std::make_unique<world::ChunkManager>();
}

Application::~Application() {
    glfwTerminate();
}

void Application::Init() {
    renderer_->Init();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
}

void Application::Run() {
    while (is_running_ && !window_->ShouldClose()) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        float delta_time = static_cast<float>((now - last_frame_time_).count()) * 1e-9f;
        last_frame_time_ = now;

        Update(delta_time);
        Draw();
    }
}

void Application::Update(float delta_time) {
    glfwPollEvents();
    
    if (window_->HasChangedSize()) {
        int width = window_->GetWidth();
        int height = window_->GetHeight();
        camera_->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }

    input_manager_->Update(delta_time);
    chunk_manager_->Update(camera_->GetPosition());
}

void Application::Draw() {
    renderer_->Draw(camera_.get(), chunk_manager_->GetMap());
    window_->SwapBuffers();
}

} // namespace voxels::application
