#pragma once
#include "types.h"
#include <chrono>
#include <cstdio>

class PlayerTimer {
public:
    void init(int time_sec, int increment_sec, TimerMode mode) {
        initial_ms_ = static_cast<int64_t>(time_sec) * 1000;
        remaining_ms_ = initial_ms_;
        increment_ms_ = static_cast<int64_t>(increment_sec) * 1000;
        mode_ = mode;
        running_ = false;
        timed_out_ = false;
        delay_remaining_ = 0;
    }

    void reset() {
        remaining_ms_ = initial_ms_;
        running_ = false;
        timed_out_ = false;
        delay_remaining_ = 0;
    }

    void start() {
        if (timed_out_) return;
        running_ = true;
        last_tick_ = std::chrono::steady_clock::now();
        if (mode_ == TimerMode::BRONSTEIN)
            delay_remaining_ = increment_ms_;
    }

    void stop(bool apply_increment = true) {
        if (!running_) return;
        tick();
        running_ = false;
        if (apply_increment && mode_ == TimerMode::FISCHER && !timed_out_)
            remaining_ms_ += increment_ms_;
    }

    void tick() {
        if (!running_ || timed_out_) return;
        auto now = std::chrono::steady_clock::now();
        int64_t elapsed = std::chrono::duration_cast<
            std::chrono::milliseconds>(now - last_tick_).count();
        last_tick_ = now;

        if (mode_ == TimerMode::BRONSTEIN && delay_remaining_ > 0) {
            delay_remaining_ -= elapsed;
            if (delay_remaining_ < 0) {
                remaining_ms_ += delay_remaining_;
                delay_remaining_ = 0;
            }
        } else {
            remaining_ms_ -= elapsed;
        }

        if (remaining_ms_ <= 0) {
            remaining_ms_ = 0;
            timed_out_ = true;
            running_ = false;
        }
    }

    bool is_running() const { return running_; }
    bool is_timed_out() const { return timed_out_; }
    int64_t remaining_ms() const { return remaining_ms_; }

    std::string format_time() const {
        if (remaining_ms_ <= 0) return "0:00";
        int total_sec = static_cast<int>(remaining_ms_ / 1000);
        int tenths = static_cast<int>((remaining_ms_ % 1000) / 100);
        int minutes = total_sec / 60;
        int seconds = total_sec % 60;

        char buf[32];
        if (remaining_ms_ < 10000)
            std::snprintf(buf, sizeof(buf), "%d:%02d.%d", minutes, seconds, tenths);
        else
            std::snprintf(buf, sizeof(buf), "%d:%02d", minutes, seconds);
        return buf;
    }

private:
    int64_t initial_ms_ = 0;
    int64_t remaining_ms_ = 0;
    int64_t increment_ms_ = 0;
    int64_t delay_remaining_ = 0;
    TimerMode mode_ = TimerMode::SUDDEN_DEATH;
    bool running_ = false;
    bool timed_out_ = false;
    std::chrono::steady_clock::time_point last_tick_;
};
