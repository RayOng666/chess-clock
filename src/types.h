#pragma once
#include <cstdint>
#include <string>

enum class TimerMode { SUDDEN_DEATH, FISCHER, BRONSTEIN };
enum class ClockState { IDLE, RUNNING, PAUSED, TIMEOUT };

struct TimeControl {
    int main_time_sec = 300;
    int increment_sec = 0;
    TimerMode mode = TimerMode::SUDDEN_DEATH;
};

#ifdef _WIN32
#include <windows.h>
inline void system_beep() { MessageBeep(MB_ICONEXCLAMATION); }
#else
#include <cstdio>
inline void system_beep() { putchar('\a'); fflush(stdout); }
#endif
