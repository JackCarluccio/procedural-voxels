#include "graphics/window.h"

#include "application/application.h"
#include "graphics/graphics_core.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace voxels::graphics {

	Window::Window(application::Application* app, int width, int height, const std::string& title, GLFWmonitor* monitor)
		: width_(width),
		height_(height),
		title_(title),
		window_(glfwCreateWindow(width, height, title_.c_str(), monitor, nullptr))
	{
		if (window_ == nullptr) {
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetWindowUserPointer(window_, app);
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

		return *this;
	}

	void Window::MakeContextCurrent() noexcept {
		glfwMakeContextCurrent(window_);
	}

	void Window::SwapBuffers() noexcept {
		glfwSwapBuffers(window_);
	}
	
	bool Window::ShouldClose() const noexcept {
		return glfwWindowShouldClose(window_);
	}

	bool Window::HarvestHasChangedSize() noexcept {
		bool changed = hasChangedSize_;
		hasChangedSize_ = false;
		return changed;
	}

	void Window::framebuffer_size_callback(GLFWwindow* handle, int width, int height) noexcept {
		auto* app = static_cast<application::Application*>(glfwGetWindowUserPointer(handle));
		Window& window = app->GetWindow();
		window.OnResize(width, height);
	}

	void Window::OnResize(int width, int height) noexcept {
		width_ = width;
		height_ = height;
		hasChangedSize_ = true; 
		glViewport(0, 0, width, height);
	}

}
