#pragma once

#include <string>

struct GLFWwindow;

namespace Graphics {

  class Window {
  public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // Windows own unique resources and therefore should not be copied
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    // Allow moving of windows
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    void makeContextCurrent() const noexcept;
    bool shouldClose() const noexcept;
    void swapBuffers() const noexcept;

    int getWidth() const noexcept { return width_; }
    int getHeight() const noexcept { return height_; }
    const std::string& getTitle() const noexcept { return title_; }

  private:
    // Callback for GLFW
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) noexcept;

    void onResize(int width, int height) noexcept;

    int width_;
    int height_;
    std::string title_;
    GLFWwindow* window_;
  };

}
