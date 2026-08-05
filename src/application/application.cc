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

    Application::Application(const Settings& settings)
        : settings_(settings),
        start_time_(std::chrono::steady_clock::now())
    {
        InitGLFW();
        ConfigureGLFW();

        CreateWindow();
        
        LoadOpenGL();
        
        CreateScene();

        renderer_ = std::make_unique<graphics::Renderer>();
        input_manager_ = std::make_unique<input::InputManager>(settings_, window_->GetGLFWwindow(), scene_->GetCamera());
        chunk_manager_ = std::make_unique<world::ChunkManager>();
    }

    Application::~Application() {
        glfwTerminate();
    }

    void Application::Init() {
        renderer_->Init(settings_);
        chunk_manager_->Init();
    }

    void Application::Run() {
        is_running_ = true;
        while (is_running_ && !window_->ShouldClose()) {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            float delta_time = static_cast<float>((now - last_frame_time_).count()) * 1e-9f;
            
            frame_++;
            last_frame_time_ = now;

            Update(delta_time);
            Draw();
        }

        is_running_ = false;
    }

    void Application::Update(float delta_time) {
        glfwPollEvents();
        
        if (window_->HasChangedSize()) {
            scene_->GetCamera().SetAspectRatio(window_->GetAspectRatio());
        }

        input_manager_->Update(delta_time);
        chunk_manager_->Update(scene_->GetCamera().GetPosition());
    }

    void Application::Draw() {
        renderer_->Draw(*scene_, chunk_manager_->GetMap());
        window_->SwapBuffers();
    }

    void Application::InitGLFW() {
        auto result = glfwInit();
        if (result == GLFW_FALSE) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    void Application::ConfigureGLFW() {
        // Use OpenGL 4.6 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        // Enable sRGB framebuffer to allow for gamma correction
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    }

    void Application::CreateWindow() {
        // Create a fullscreen window
        GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window_ = std::make_unique<graphics::Window>(
            mode->width,
            mode->height,
            settings_.app_settings.title,
            primary_monitor
        );
        window_->MakeContextCurrent();
    }

    void Application::LoadOpenGL() {
        auto result = gladLoadGL(glfwGetProcAddress);
        if (result == 0) {
            throw std::runtime_error("Failed to initialize OpenGL loader");
        }
    }

    void Application::CreateScene() {
        graphics::Camera camera(
            glm::radians(45.0f),
            window_->GetAspectRatio(),
            0.1f,
            5000.0f
        );

        graphics::Skybox skybox(graphics::Color(143, 217, 251));

        scene_ = std::make_unique<graphics::Scene>(
            std::move(camera),
            std::move(skybox)
        );
    }

}
