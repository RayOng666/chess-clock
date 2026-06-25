#include "chess_clock.h"
#include "settings_dialog.h"
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Menu_Item.H>
#include <cstdio>

#ifdef _WIN32
#include <FL/platform.H>
#endif

static Fl_Color bg_active()   { return fl_rgb_color(0, 140, 65); }
static Fl_Color bg_inactive() { return fl_rgb_color(48, 48, 48); }
static Fl_Color bg_timeout()  { return fl_rgb_color(190, 30, 30); }
static Fl_Color bg_dark()     { return fl_rgb_color(30, 30, 30); }
static Fl_Color ctrl_bg()     { return fl_rgb_color(38, 38, 38); }
static Fl_Color clr_warn()    { return fl_rgb_color(255, 55, 55); }
static Fl_Color btn_bg()      { return fl_rgb_color(60, 60, 60); }

static void style_btn(Fl_Button* b) {
    b->box(FL_FLAT_BOX);
    b->color(btn_bg());
    b->labelcolor(FL_WHITE);
    b->labelsize(13);
}

ChessClock::ChessClock(const AppConfig& cfg)
    : Fl_Double_Window(cfg.window_x, cfg.window_y, W, H,
          "\xe6\xa3\x8b\xe9\x92\x9f")
    , config_(cfg)
{
    color(bg_dark());
    size_range(W, H, W, H);

    int bw = 70, bh = 32, by = H - CTRL_H + 9, gap = 12;
    int total = bw * 6 + gap * 5;
    int bx = (W - total) / 2;

    btn_topmost_ = new Fl_Button(bx, by, bw, bh,
        "\xe7\xbd\xae\xe9\xa1\xb6"); style_btn(btn_topmost_);
    btn_topmost_->callback(on_topmost, this);
    bx += bw + gap;

    btn_mute_ = new Fl_Button(bx, by, bw, bh,
        "\xe5\xa3\xb0\xe9\x9f\xb3"); style_btn(btn_mute_);
    btn_mute_->callback(on_mute, this);
    bx += bw + gap;

    btn_preset_ = new Fl_Button(bx, by, bw, bh,
        "\xe9\xa2\x84\xe8\xae\xbe"); style_btn(btn_preset_);
    btn_preset_->callback(on_preset, this);
    bx += bw + gap;

    btn_settings_ = new Fl_Button(bx, by, bw, bh,
        "\xe8\xae\xbe\xe7\xbd\xae"); style_btn(btn_settings_);
    btn_settings_->callback(on_settings, this);
    bx += bw + gap;

    btn_start_pause_ = new Fl_Button(bx, by, bw, bh,
        "\xe5\xbc\x80\xe5\xa7\x8b"); style_btn(btn_start_pause_);
    btn_start_pause_->callback(on_start_pause, this);
    bx += bw + gap;

    btn_reset_ = new Fl_Button(bx, by, bw, bh,
        "\xe9\x87\x8d\xe7\xbd\xae"); style_btn(btn_reset_);
    btn_reset_->callback(on_reset, this);

    end();
    callback(close_cb, this);
    apply_config(cfg.time_control);

    if (cfg.always_on_top) set_topmost(true);
    if (cfg.muted) btn_mute_->label(
        "\xe9\x9d\x99\xe9\x9f\xb3");
}

// ── drawing ─────────────────────────────────────────────
void ChessClock::draw() {
    Fl_Double_Window::draw();

    int hw = W / 2, th = H - CTRL_H;

    for (int i = 0; i < 2; i++) {
        int px = i * hw;
        Fl_Color bg;
        if (timers_[i].is_timed_out())
            bg = ((flash_counter_ / 10) % 2) ? bg_dark() : bg_timeout();
        else if (state_ == ClockState::RUNNING && current_player_ == i)
            bg = bg_active();
        else
            bg = bg_inactive();
        fl_rectf(px, 0, hw, th, bg);

        // player label
        const char* name = i == 0
            ? "\xe7\x99\xbd\xe6\x96\xb9"   // 白方
            : "\xe9\xbb\x91\xe6\x96\xb9";  // 黑方
        fl_color(fl_rgb_color(180, 180, 180));
        fl_font(FL_HELVETICA, 20);
        int lw = 0, lh = 0;
        fl_measure(name, lw, lh);
        fl_draw(name, px + (hw - lw) / 2, 45);

        // active indicator dot
        int cx = px + hw / 2, cy = 62;
        if (state_ == ClockState::RUNNING && current_player_ == i) {
            fl_color(FL_WHITE);
            fl_pie(cx - 5, cy, 10, 10, 0, 360);
        } else {
            fl_color(fl_rgb_color(120, 120, 120));
            fl_arc(cx - 5, cy, 10, 10, 0, 360);
        }

        // time string
        std::string ts = timers_[i].format_time();
        Fl_Color tc = FL_WHITE;
        if (!timers_[i].is_timed_out() && timers_[i].remaining_ms() < 10000)
            tc = clr_warn();
        fl_color(tc);
        fl_font(FL_HELVETICA_BOLD, 68);
        int tw = 0, tth = 0;
        fl_measure(ts.c_str(), tw, tth);
        fl_draw(ts.c_str(), px + (hw - tw) / 2, th / 2 + tth / 4 + 25);
    }

    fl_color(fl_rgb_color(80, 80, 80));
    fl_line(hw, 0, hw, th);
    fl_rectf(0, th, W, CTRL_H, ctrl_bg());
}

// ── event handling ──────────────────────────────────────
int ChessClock::handle(int event) {
    switch (event) {
    case FL_PUSH: {
        int my = Fl::event_y();
        if (my >= H - CTRL_H || state_ == ClockState::TIMEOUT) break;
        int mx = Fl::event_x();
        switch_to(mx < W / 2 ? 1 : 0);
        return 1;
    }
    case FL_KEYBOARD: {
        int key = Fl::event_key();
        if (key == ' ') {
            if (state_ == ClockState::IDLE) switch_to(0);
            else if (state_ == ClockState::RUNNING) switch_to(1 - current_player_);
            return 1;
        }
        if (key == 'p' || key == 'P') { toggle_pause(); return 1; }
        if (key == 'r' || key == 'R') { reset_clocks(); return 1; }
        break;
    }
    }
    return Fl_Double_Window::handle(event);
}

// ── timer logic ─────────────────────────────────────────
void ChessClock::timer_cb(void* data) {
    auto* self = static_cast<ChessClock*>(data);
    self->flash_counter_++;

    if (self->state_ == ClockState::RUNNING) {
        self->timers_[self->current_player_].tick();
        if (self->timers_[self->current_player_].is_timed_out()) {
            self->state_ = ClockState::TIMEOUT;
            if (!self->config_.muted) system_beep();
            self->btn_start_pause_->label(
                "\xe8\xb6\x85\xe6\x97\xb6"); // 超时
        } else if (!self->config_.muted &&
                   self->timers_[self->current_player_].remaining_ms() < 10000 &&
                   self->flash_counter_ % 20 == 0) {
            system_beep();
        }
    }

    self->redraw();
    if (self->state_ == ClockState::RUNNING || self->state_ == ClockState::TIMEOUT)
        Fl::repeat_timeout(0.05, timer_cb, data);
}

void ChessClock::switch_to(int player) {
    if (state_ == ClockState::TIMEOUT) return;

    if (state_ == ClockState::IDLE) {
        state_ = ClockState::RUNNING;
        current_player_ = player;
        timers_[player].start();
        btn_start_pause_->label("\xe6\x9a\x82\xe5\x81\x9c"); // 暂停
        Fl::add_timeout(0.05, timer_cb, this);
    } else if (state_ == ClockState::RUNNING && current_player_ != player) {
        timers_[current_player_].stop();
        current_player_ = player;
        timers_[player].start();
    } else if (state_ == ClockState::PAUSED) {
        state_ = ClockState::RUNNING;
        current_player_ = player;
        timers_[player].start();
        btn_start_pause_->label("\xe6\x9a\x82\xe5\x81\x9c");
        Fl::add_timeout(0.05, timer_cb, this);
    }
    redraw();
}

void ChessClock::toggle_pause() {
    if (state_ == ClockState::RUNNING) {
        timers_[current_player_].stop(false);
        state_ = ClockState::PAUSED;
        Fl::remove_timeout(timer_cb, this);
        btn_start_pause_->label("\xe7\xbb\xa7\xe7\xbb\xad"); // 继续
    } else if (state_ == ClockState::PAUSED) {
        state_ = ClockState::RUNNING;
        timers_[current_player_].start();
        btn_start_pause_->label("\xe6\x9a\x82\xe5\x81\x9c");
        Fl::add_timeout(0.05, timer_cb, this);
    }
    redraw();
}

void ChessClock::reset_clocks() {
    Fl::remove_timeout(timer_cb, this);
    state_ = ClockState::IDLE;
    current_player_ = 0;
    timers_[0].reset();
    timers_[1].reset();
    flash_counter_ = 0;
    btn_start_pause_->label("\xe5\xbc\x80\xe5\xa7\x8b"); // 开始
    redraw();
}

void ChessClock::apply_config(const TimeControl& tc) {
    Fl::remove_timeout(timer_cb, this);
    config_.time_control = tc;
    timers_[0].init(tc.main_time_sec, tc.increment_sec, tc.mode);
    timers_[1].init(tc.main_time_sec, tc.increment_sec, tc.mode);
    state_ = ClockState::IDLE;
    current_player_ = 0;
    flash_counter_ = 0;
    btn_start_pause_->label("\xe5\xbc\x80\xe5\xa7\x8b");
    update_title();
    redraw();
}

void ChessClock::update_title() {
    static char title[64];
    auto& tc = config_.time_control;
    int m = tc.main_time_sec / 60, s = tc.main_time_sec % 60;
    const char* ms = "";
    if (tc.mode == TimerMode::FISCHER) ms = " Fischer";
    else if (tc.mode == TimerMode::BRONSTEIN) ms = " Bronstein";

    if (tc.increment_sec > 0) {
        if (s > 0) std::snprintf(title, 64, "\xe6\xa3\x8b\xe9\x92\x9f %d:%02d+%ds%s", m, s, tc.increment_sec, ms);
        else       std::snprintf(title, 64, "\xe6\xa3\x8b\xe9\x92\x9f %dmin+%ds%s", m, tc.increment_sec, ms);
    } else {
        if (s > 0) std::snprintf(title, 64, "\xe6\xa3\x8b\xe9\x92\x9f %d:%02d%s", m, s, ms);
        else       std::snprintf(title, 64, "\xe6\xa3\x8b\xe9\x92\x9f %dmin%s", m, ms);
    }
    label(title);
}

void ChessClock::save_config() {
    config_.window_x = x();
    config_.window_y = y();
    Prefs::save(Prefs::config_path(), config_);
}

void ChessClock::set_topmost(bool on) {
    config_.always_on_top = on;
#ifdef _WIN32
    HWND hwnd = fl_xid(this);
    if (hwnd)
        SetWindowPos(hwnd, on ? HWND_TOPMOST : HWND_NOTOPMOST,
                     0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif
    btn_topmost_->label(on
        ? "\xe2\x98\x85\xe7\xbd\xae\xe9\xa1\xb6"   // ★置顶
        : "\xe7\xbd\xae\xe9\xa1\xb6");              // 置顶
}

// ── button callbacks ────────────────────────────────────
void ChessClock::on_preset(Fl_Widget* w, void* data) {
    auto* self = static_cast<ChessClock*>(data);
    static const Fl_Menu_Item items[] = {
        {"1+0  (1\xe5\x88\x86\xe9\x92\x9f)"},
        {"3+0  (3\xe5\x88\x86\xe9\x92\x9f)"},
        {"5+0  (5\xe5\x88\x86\xe9\x92\x9f)"},
        {"3+2  Fischer"},
        {"5+3  Fischer"},
        {"10+0 (10\xe5\x88\x86\xe9\x92\x9f)"},
        {nullptr}
    };
    static const struct { int t; int i; TimerMode m; } vals[] = {
        {60,0,TimerMode::SUDDEN_DEATH}, {180,0,TimerMode::SUDDEN_DEATH},
        {300,0,TimerMode::SUDDEN_DEATH},{180,2,TimerMode::FISCHER},
        {300,3,TimerMode::FISCHER},     {600,0,TimerMode::SUDDEN_DEATH}
    };
    auto* pick = items->popup(w->x(), w->y());
    if (!pick) return;
    int idx = static_cast<int>(pick - items);
    if (idx < 0 || idx > 5) return;
    TimeControl tc{vals[idx].t, vals[idx].i, vals[idx].m};
    self->apply_config(tc);
}

void ChessClock::on_settings(Fl_Widget*, void* data) {
    auto* self = static_cast<ChessClock*>(data);
    SettingsDialog dlg;
    TimeControl tc = self->config_.time_control;
    if (dlg.run(tc)) self->apply_config(tc);
}

void ChessClock::on_start_pause(Fl_Widget*, void* data) {
    auto* self = static_cast<ChessClock*>(data);
    if (self->state_ == ClockState::IDLE)
        self->switch_to(0);
    else
        self->toggle_pause();
}

void ChessClock::on_reset(Fl_Widget*, void* data) {
    static_cast<ChessClock*>(data)->reset_clocks();
}

void ChessClock::on_topmost(Fl_Widget*, void* data) {
    auto* self = static_cast<ChessClock*>(data);
    self->set_topmost(!self->config_.always_on_top);
}

void ChessClock::on_mute(Fl_Widget*, void* data) {
    auto* self = static_cast<ChessClock*>(data);
    self->config_.muted = !self->config_.muted;
    self->btn_mute_->label(self->config_.muted
        ? "\xe9\x9d\x99\xe9\x9f\xb3"   // 静音
        : "\xe5\xa3\xb0\xe9\x9f\xb3"); // 声音
}

void ChessClock::close_cb(Fl_Widget* w, void*) {
    auto* self = static_cast<ChessClock*>(w);
    self->save_config();
    self->hide();
}
