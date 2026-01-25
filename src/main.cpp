#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

int main() {
	// Load OpenGL 4.6 Core
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Load OpenGL functions
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialize OpenGL loader" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	// Main loop
	while(!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
