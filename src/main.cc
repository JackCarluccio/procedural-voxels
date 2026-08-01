#include "application/application.h"

#include <iostream>
#include <stdexcept>

int main() {
	try {
		voxels::application::Settings settings = {
			.app_settings {
				.title = "Voxels",
			},

			.user_settings {
				.vsync = false,
			}
		};

		voxels::application::Application app(settings);
		app.Init();
		app.Run();
	} catch (const std::exception& e) {
		std::cerr << "Application error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
