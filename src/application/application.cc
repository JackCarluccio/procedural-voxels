#include "application/application.h"

#include "graphics/graphics_core.h"

#include <stdexcept>

namespace voxels::application {

    Application::Application(int width, int height, const std::string& title)
        : is_running_(true)
    {
        // Initialize GLFW
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // Configure GLFW for OpenGL 4.6 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create application window
        window_ = std::make_unique<graphics::Window>(width, height, title);
        window_->MakeContextCurrent();

        // Load OpenGL functions using GLAD
        if (!gladLoadGL(glfwGetProcAddress)) {
		    throw std::runtime_error("Failed to initialize OpenGL loader");
	    }
    }

    Application::~Application() {
        glfwTerminate();
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window_->SwapBuffers();
    }

} // namespace voxels::application