#include "graphics/Window.h"

#include "graphics/GraphicsCore.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace Graphics {

  Window::Window(int width, int height, const std::string& title)
    : width_(width),
      height_(height),
      title_(title),
      window_(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr))
    {
    // Window creation could fail
    if (window_ == nullptr) {
      throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
  }

  Window::~Window() {
    glfwDestroyWindow(window_);
  }

  Window::Window(Window&& other) noexcept
    : width_(other.width_),
      height_(other.height_), 
      title_(std::move(other.title_)),
      window_(other.window_) {
    // Update the GLFW user pointer to the new cpp instance
    if (window_) {
        glfwSetWindowUserPointer(window_, this);
    }

    other.window_ = nullptr; // Remove resource from other window
  }

  Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
      // Clean up existing window
      if (window_) {
        glfwDestroyWindow(window_);
      }
      
      width_ = other.width_;
      height_ = other.height_;
      title_ = std::move(other.title_);
      window_ = other.window_;

      // Update the GLFW user pointer to the new cpp instance
      if (window_) {
        glfwSetWindowUserPointer(window_, this);
      }
      
      other.window_ = nullptr; // Remove resource from other window
    }

    return *this;
  }

  void Window::makeContextCurrent() const noexcept {
    glfwMakeContextCurrent(window_);
  }

  bool Window::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_);
  }

  void Window::swapBuffers() const noexcept {
    return glfwSwapBuffers(window_);
  }

  void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) noexcept {
    // Find the cpp instance from GLFW user pointer
    Window* windowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (windowClass) {
      windowClass->onResize(width, height);
    }
  }

  void Window::onResize(int width, int height) noexcept {
    width_ = width;
    height_ = height;
    glViewport(0, 0, width, height);
  }

}
