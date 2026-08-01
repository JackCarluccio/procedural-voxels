#pragma once

#include <string>

namespace voxels::application {

    struct AppSettings {
        std::string title;
    };

    struct UserSettings {
        bool vsync;
    };

    struct Settings {
        AppSettings app_settings;
        UserSettings user_settings;
    };

}
