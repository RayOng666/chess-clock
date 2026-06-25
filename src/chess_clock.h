#pragma once
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include "player_timer.h"
#include "prefs.h"

class ChessClock : public Fl_Double_Window {
public:
    explicit ChessClock(const AppConfig& cfg);

private:
    void draw() override;
    int handle(int event) override;

    static void timer_cb(void* data);
    static void on_preset(Fl_Widget* w, void* data);
    static void on_settings(Fl_Widget* w, void* data);
    static void on_start_pause(Fl_Widget* w, void* data);
    static void on_reset(Fl_Widget* w, void* data);
    static void on_topmost(Fl_Widget* w, void* data);
    static void on_mute(Fl_Widget* w, void* data);
    static void close_cb(Fl_Widget* w, void* data);

    void switch_to(int player);
    void toggle_pause();
    void reset_clocks();
    void apply_config(const TimeControl& tc);
    void update_title();
    void save_config();
    void set_topmost(bool on);

    ClockState state_ = ClockState::IDLE;
    int current_player_ = 0;
    PlayerTimer timers_[2];
    AppConfig config_;

    Fl_Button* btn_preset_;
    Fl_Button* btn_settings_;
    Fl_Button* btn_start_pause_;
    Fl_Button* btn_reset_;
    Fl_Button* btn_topmost_;
    Fl_Button* btn_mute_;

    int flash_counter_ = 0;

    static constexpr int W = 600;
    static constexpr int H = 350;
    static constexpr int CTRL_H = 50;
};
