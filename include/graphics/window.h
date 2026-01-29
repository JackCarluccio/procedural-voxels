#pragma once

#include <string>

struct GLFWwindow;

namespace graphics {

  class Window {
  public:
    explicit Window(int width, int height, const std::string& title);
    ~Window();

    // Windows own unique resources and therefore should not be copied
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    // Allow moving of windows
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    void MakeContextCurrent() const noexcept;
    bool ShouldClose() const noexcept;
    void SwapBuffers() const noexcept;

    int GetWidth() const noexcept { return width_; }
    int GetHeight() const noexcept { return height_; }
    const std::string& GetTitle() const noexcept { return title_; }

  private:
    // Callback for GLFW
    static void framebuffer_size_callback(GLFWwindow* handle, int width, int height) noexcept;

    void OnResize(int width, int height) noexcept;

    int width_;
    int height_;
    std::string title_;
    GLFWwindow* window_;
  };

}
