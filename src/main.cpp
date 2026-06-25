#include "chess_clock.h"
#include "prefs.h"
#include <FL/Fl.H>

int main(int argc, char* argv[]) {
    Fl::scheme("gtk+");

    auto cfg = Prefs::load(Prefs::config_path());

    ChessClock win(cfg);
    win.show(argc, argv);

    return Fl::run();
}
