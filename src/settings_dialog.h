#pragma once
#include "types.h"
#include <FL/Fl_Window.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>

class SettingsDialog : public Fl_Window {
public:
    SettingsDialog();
    bool run(TimeControl& tc);
    void set_values(int time_sec, int incr_sec, TimerMode mode);

private:
    static void on_ok(Fl_Widget*, void*);
    static void on_cancel(Fl_Widget*, void*);
    static void on_preset(Fl_Widget* w, void* idx);

    Fl_Int_Input* input_min_;
    Fl_Int_Input* input_sec_;
    Fl_Int_Input* input_incr_;
    Fl_Choice* choice_mode_;
    bool ok_pressed_ = false;
};
