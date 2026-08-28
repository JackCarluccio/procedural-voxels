#pragma once

#include <string>

namespace voxels::application {

    struct AppSettings {
        std::string title;
    };

    struct UserSettings {
        bool vsync;

        float camera_fov;
        float camera_speed;
        float camera_sensitivity;
    };

    struct Settings {
        AppSettings app_settings;
        UserSettings user_settings;
    };

}
