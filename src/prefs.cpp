#include "prefs.h"
#include <fstream>
#include <map>

#ifdef _WIN32
#include <windows.h>
#endif

std::string Prefs::config_path() {
#ifdef _WIN32
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string path(buf, len);
    auto pos = path.find_last_of("\\/");
    if (pos != std::string::npos) path = path.substr(0, pos);
    return path + "\\clock.conf";
#else
    return "./clock.conf";
#endif
}

AppConfig Prefs::load(const std::string& path) {
    AppConfig cfg;
    std::ifstream f(path);
    if (!f.is_open()) return cfg;

    std::map<std::string, std::string> kv;
    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        auto key = line.substr(0, eq);
        auto val = line.substr(eq + 1);
        while (!key.empty() && key.back() == ' ') key.pop_back();
        while (!val.empty() && val.front() == ' ') val.erase(val.begin());
        kv[key] = val;
    }

    auto get_int = [&](const std::string& k, int d) {
        auto it = kv.find(k);
        if (it == kv.end()) return d;
        try { return std::stoi(it->second); } catch (...) { return d; }
    };
    auto get_bool = [&](const std::string& k, bool d) {
        auto it = kv.find(k);
        if (it == kv.end()) return d;
        return it->second == "1" || it->second == "true";
    };

    cfg.time_control.main_time_sec = get_int("main_time", 300);
    cfg.time_control.increment_sec = get_int("increment", 0);

    auto it = kv.find("mode");
    if (it != kv.end()) {
        if (it->second == "fischer") cfg.time_control.mode = TimerMode::FISCHER;
        else if (it->second == "bronstein") cfg.time_control.mode = TimerMode::BRONSTEIN;
    }

    cfg.window_x = get_int("window_x", 100);
    cfg.window_y = get_int("window_y", 100);
    cfg.always_on_top = get_bool("always_on_top", false);
    cfg.muted = get_bool("muted", false);
    return cfg;
}

void Prefs::save(const std::string& path, const AppConfig& cfg) {
    std::ofstream f(path);
    if (!f.is_open()) return;

    f << "main_time=" << cfg.time_control.main_time_sec << "\n";
    f << "increment=" << cfg.time_control.increment_sec << "\n";

    const char* ms = "sudden_death";
    if (cfg.time_control.mode == TimerMode::FISCHER) ms = "fischer";
    else if (cfg.time_control.mode == TimerMode::BRONSTEIN) ms = "bronstein";
    f << "mode=" << ms << "\n";

    f << "window_x=" << cfg.window_x << "\n";
    f << "window_y=" << cfg.window_y << "\n";
    f << "always_on_top=" << (cfg.always_on_top ? 1 : 0) << "\n";
    f << "muted=" << (cfg.muted ? 1 : 0) << "\n";
}
