#include "settings_dialog.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <cstdlib>
#include <cstring>
#include <cstdio>

struct Preset {
    const char* label;
    int time_sec;
    int incr_sec;
    TimerMode mode;
};

static const Preset PRESETS[] = {
    {"1+0",   60, 0, TimerMode::SUDDEN_DEATH},
    {"3+0",  180, 0, TimerMode::SUDDEN_DEATH},
    {"5+0",  300, 0, TimerMode::SUDDEN_DEATH},
    {"3+2",  180, 2, TimerMode::FISCHER},
    {"5+3",  300, 3, TimerMode::FISCHER},
    {"10+0", 600, 0, TimerMode::SUDDEN_DEATH},
};
static constexpr int NUM_PRESETS = 6;

SettingsDialog::SettingsDialog()
    : Fl_Window(340, 240, "Settings")
{
    auto* lbl_preset = new Fl_Box(10, 10, 80, 25, "Presets:");
    lbl_preset->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    int bx = 10;
    for (int i = 0; i < NUM_PRESETS; i++) {
        auto* btn = new Fl_Button(bx, 38, 50, 28, PRESETS[i].label);
        btn->callback(on_preset, reinterpret_cast<void*>(static_cast<intptr_t>(i)));
        btn->box(FL_FLAT_BOX);
        btn->color(fl_rgb_color(70, 70, 70));
        btn->labelcolor(FL_WHITE);
        btn->labelsize(13);
        bx += 54;
    }

    auto* lbl_mode = new Fl_Box(10, 80, 60, 25, "Mode:");
    lbl_mode->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    choice_mode_ = new Fl_Choice(75, 80, 180, 25);
    choice_mode_->add("Sudden Death");
    choice_mode_->add("Fischer");
    choice_mode_->add("Bronstein");
    choice_mode_->value(0);

    auto* lbl_time = new Fl_Box(10, 118, 60, 25, "Time:");
    lbl_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    input_min_ = new Fl_Int_Input(75, 118, 55, 25);
    auto* lbl_min = new Fl_Box(132, 118, 30, 25, "min");
    lbl_min->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    input_sec_ = new Fl_Int_Input(168, 118, 55, 25);
    auto* lbl_sec = new Fl_Box(225, 118, 30, 25, "sec");
    lbl_sec->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    auto* lbl_incr = new Fl_Box(10, 153, 60, 25, "Incr:");
    lbl_incr->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    input_incr_ = new Fl_Int_Input(75, 153, 55, 25);
    auto* lbl_is = new Fl_Box(132, 153, 30, 25, "sec");
    lbl_is->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    auto* btn_ok = new Fl_Button(80, 195, 80, 32, "OK");
    btn_ok->callback(on_ok, this);
    btn_ok->box(FL_FLAT_BOX);
    btn_ok->color(fl_rgb_color(0, 120, 60));
    btn_ok->labelcolor(FL_WHITE);

    auto* btn_cancel = new Fl_Button(180, 195, 80, 32, "Cancel");
    btn_cancel->callback(on_cancel, this);
    btn_cancel->box(FL_FLAT_BOX);
    btn_cancel->color(fl_rgb_color(100, 100, 100));
    btn_cancel->labelcolor(FL_WHITE);

    end();
    set_modal();
    color(fl_rgb_color(45, 45, 45));

    input_min_->value("5");
    input_sec_->value("0");
    input_incr_->value("0");
}

void SettingsDialog::set_values(int time_sec, int incr_sec, TimerMode mode) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%d", time_sec / 60);
    input_min_->value(buf);
    std::snprintf(buf, sizeof(buf), "%d", time_sec % 60);
    input_sec_->value(buf);
    std::snprintf(buf, sizeof(buf), "%d", incr_sec);
    input_incr_->value(buf);

    int idx = 0;
    if (mode == TimerMode::FISCHER) idx = 1;
    else if (mode == TimerMode::BRONSTEIN) idx = 2;
    choice_mode_->value(idx);
}

bool SettingsDialog::run(TimeControl& tc) {
    set_values(tc.main_time_sec, tc.increment_sec, tc.mode);
    ok_pressed_ = false;
    show();
    while (shown()) Fl::wait();

    if (ok_pressed_) {
        int m = std::atoi(input_min_->value());
        int s = std::atoi(input_sec_->value());
        tc.main_time_sec = m * 60 + s;
        tc.increment_sec = std::atoi(input_incr_->value());
        switch (choice_mode_->value()) {
        case 1:  tc.mode = TimerMode::FISCHER;   break;
        case 2:  tc.mode = TimerMode::BRONSTEIN; break;
        default: tc.mode = TimerMode::SUDDEN_DEATH; break;
        }
    }
    return ok_pressed_;
}

void SettingsDialog::on_ok(Fl_Widget*, void* d) {
    static_cast<SettingsDialog*>(d)->ok_pressed_ = true;
    static_cast<SettingsDialog*>(d)->hide();
}

void SettingsDialog::on_cancel(Fl_Widget*, void* d) {
    static_cast<SettingsDialog*>(d)->hide();
}

void SettingsDialog::on_preset(Fl_Widget* w, void* idx_ptr) {
    int idx = static_cast<int>(reinterpret_cast<intptr_t>(idx_ptr));
    if (idx < 0 || idx >= NUM_PRESETS) return;
    auto* dlg = static_cast<SettingsDialog*>(w->window());
    dlg->set_values(PRESETS[idx].time_sec, PRESETS[idx].incr_sec, PRESETS[idx].mode);
}
