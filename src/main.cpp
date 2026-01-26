#include "graphics/GraphicsCore.h"
#include "graphics/Window.h"

#include <iostream>

int main() {
	// Load OpenGL 4.6 Core
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  Graphics::Window window = Graphics::Window(800, 600, "Voxels");
	window.makeContextCurrent();

	// Load OpenGL functions
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialize OpenGL loader" << std::endl;
		return -1;
	}

	glViewport(0, 0, window.getWidth(), window.getHeight());

	// Main loop
	while(!window.shouldClose()) {
		window.swapBuffers();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

