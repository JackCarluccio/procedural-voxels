#include "application/application.h"

#include "graphics/camera.h"
#include "graphics/color.h"
#include "graphics/graphics_core.h"
#include "graphics/renderer.h"
#include "graphics/skybox.h"
#include "graphics/window.h"
#include "input/input_manager.h"
#include "world/chunk_manager.h"

#include <stdexcept>
#include <utility>

namespace voxels::application {

    Application::Application(const std::string& title)
        : is_running_(true)
    {
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // Configure GLFW for OpenGL 4.6 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a fullscreen window
        GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        // Enable sRGB framebuffer to allow for gamma correction
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

        window_ = std::make_unique<graphics::Window>(mode->width, mode->height, title, primary_monitor);

        // Disable the cursor while retaining mouse movement
        glfwSetInputMode(window_->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window_->GetGLFWwindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        window_->MakeContextCurrent();
        glfwSwapInterval(0);

        // Load OpenGL functions using GLAD
        if (gladLoadGL(glfwGetProcAddress) == 0) {
            throw std::runtime_error("Failed to initialize OpenGL loader");
        }

        graphics::Camera camera(
            glm::radians(45.0f),
            static_cast<float>(mode->width) / static_cast<float>(mode->height),
            0.1f,
            5000.0f
        );

        graphics::Skybox skybox(graphics::Color(143, 217, 251));

        scene_ = graphics::Scene(std::move(camera), std::move(skybox));

        renderer_ = std::make_unique<graphics::Renderer>();
        input_manager_ = std::make_unique<input::InputManager>(window_->GetGLFWwindow(), scene_.GetCamera());
        chunk_manager_ = std::make_unique<world::ChunkManager>();
    }

    Application::~Application() {
        glfwTerminate();
    }

    void Application::Init() {
        renderer_->Init();
        chunk_manager_->Init();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_DEPTH_TEST);

        // Make OpenGL perform gamma correction before writing to the screen
        glEnable(GL_FRAMEBUFFER_SRGB);

        start_time_ = std::chrono::steady_clock::now();
    }

    void Application::Run() {
        while (is_running_ && !window_->ShouldClose()) {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            float delta_time = static_cast<float>((now - last_frame_time_).count()) * 1e-9f;
            
            frame_++;
            last_frame_time_ = now;

            Update(delta_time);
            Draw();
        }
    }

    void Application::Update(float delta_time) {
        glfwPollEvents();
        
        if (window_->HasChangedSize()) {
            float width = window_->GetWidth();
            float height = window_->GetHeight();
            scene_.GetCamera().SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        }

        input_manager_->Update(delta_time);
        chunk_manager_->Update(scene_.GetCamera().GetPosition());
    }

    void Application::Draw() {
        renderer_->Draw(scene_, chunk_manager_->GetMap());
        window_->SwapBuffers();
    }

}
