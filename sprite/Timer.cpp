#include "Timer.h"

namespace minesweeper {
    Timer::Timer(ImageRepo &imageRepo, const Layout &layout)
            : DigitPanel(imageRepo, layout.getTimerDigitPanel()), layout(layout), running(false), elapsed(0) {

    }

    SDL_Rect Timer::getDigitRect(int position) {
        return layout.getTimerDigit(position);
    }

    int Timer::getDisplayValue() {
        return running ? static_cast<int>(timer.elapsed()) : elapsed;
    }

    void Timer::onStateChange(GameState state) {
        if (state == GameState::PLAYING) {
            running = true;
            timer.reset();
        } else if (state == GameState::WON || state == GameState::LOST) {
            running = false;
            elapsed = static_cast<int>(timer.elapsed());
        } else {
            running = false;
            elapsed = 0;
        }
    }
}
