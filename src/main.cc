#include "graphics/graphics_core.h"
#include "graphics/window.h"

#include <iostream>

int main() {
	// Load OpenGL 4.6 Core
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  graphics::Window window = graphics::Window(800, 600, "Voxels");
	window.MakeContextCurrent();

	// Load OpenGL functions
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialize OpenGL loader" << std::endl;
		return -1;
	}

	glViewport(0, 0, window.GetWidth(), window.GetHeight());

	// Main loop
	while(!window.ShouldClose()) {
		window.SwapBuffers();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

