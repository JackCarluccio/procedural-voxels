#include "application/application.h"

#include <iostream>
#include <stdexcept>

int main() {
	try {
		voxels::application::Application app(800, 600, "Voxels");
		app.Run();
	} catch (const std::exception& e) {
		std::cerr << "Application error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
