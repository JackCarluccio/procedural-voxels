#include "graphics/window.h"

#include "graphics/graphics_core.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace voxels::graphics {

Window::Window(int width, int height, const std::string& title, GLFWmonitor* monitor)
	: width_(width),
	  height_(height),
	  title_(title),
	  window_(glfwCreateWindow(width, height, title_.c_str(), monitor, nullptr))
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
	window_(other.window_)
{
	other.window_ = nullptr;

	// Make sure the GLFW user pointer points to this new cpp instance
	if (window_) {
		glfwSetWindowUserPointer(window_, this);
	}

}

Window& Window::operator=(Window&& other) noexcept {
	if (this == &other) return *this;

	if (window_) {
		glfwDestroyWindow(window_);
	}
	
	width_ = other.width_;
	height_ = other.height_;
	title_ = std::move(other.title_);
	window_ = other.window_;

	other.window_ = nullptr;

	// Make sure the GLFW user pointer points to this new cpp instance
	if (window_) {
		glfwSetWindowUserPointer(window_, this);
	}

	return *this;
}

void Window::MakeContextCurrent() const noexcept {
	glfwMakeContextCurrent(window_);
}

bool Window::ShouldClose() const noexcept {
	return glfwWindowShouldClose(window_);
}

void Window::SwapBuffers() const noexcept {
	return glfwSwapBuffers(window_);
}

bool Window::HasChangedSize() noexcept {
	bool changed = hasChangedSize_;
	hasChangedSize_ = false;
	return changed;
}

void Window::framebuffer_size_callback(GLFWwindow* handle, int width, int height) noexcept {
	// Find the cpp instance from GLFW user pointer
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
	if (window) {
		window->OnResize(width, height);
	}
}

void Window::OnResize(int width, int height) noexcept {
	width_ = width;
	height_ = height;
	glViewport(0, 0, width, height);
	hasChangedSize_ = true;
}

} // namespace voxels::graphics
