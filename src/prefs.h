#pragma once
#include "types.h"
#include <string>

struct AppConfig {
    TimeControl time_control;
    int window_x = 100;
    int window_y = 100;
    bool always_on_top = false;
    bool muted = false;
};

namespace Prefs {
    AppConfig load(const std::string& path);
    void save(const std::string& path, const AppConfig& cfg);
    std::string config_path();
}
