#pragma once

#include <string>

struct GLFWwindow;
struct GLFWmonitor;

namespace voxels::graphics {

	class Window {
	public:
		explicit Window(int width, int height, const std::string& title, GLFWmonitor* monitor = nullptr);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		void MakeContextCurrent() noexcept;
		bool ShouldClose() const noexcept;
		void SwapBuffers() noexcept;

		bool HasChangedSize() noexcept;

		int GetWidth() const noexcept { return width_; }
		int GetHeight() const noexcept { return height_; }
		GLFWwindow* GetGLFWwindow() const noexcept { return window_; }
		
		float GetAspectRatio() const noexcept {
			return static_cast<float>(width_) / static_cast<float>(height_);
		}

	private:
		// Callback for GLFW window resizing events
		static void framebuffer_size_callback(GLFWwindow* handle, int width, int height) noexcept;

		void OnResize(int width, int height) noexcept;

		int width_;
		int height_;
		std::string title_;
		GLFWwindow* window_; // Must be initialized after title_
		bool hasChangedSize_;
	};

}
