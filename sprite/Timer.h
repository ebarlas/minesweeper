#ifndef MINESWEEPER_TIMER_H
#define MINESWEEPER_TIMER_H

#include "SDL.h"
#include "../util/ClockTimer.h"
#include "../config/Layout.h"
#include "../sdl/ImageRepo.h"
#include "DigitPanel.h"
#include "GameStateListener.h"

namespace minesweeper {
    class Timer : public DigitPanel, public GameStateListener {
    public:
        Timer(ImageRepo &imageRepo, const Layout &layout);
        SDL_Rect getDigitRect(int position) override;
        int getDisplayValue() override;
        void onStateChange(GameState state) override;
    private:
        const Layout &layout;
        ClockTimer timer;
        bool running;
        int elapsed;
    };

    using TimerPtr = std::shared_ptr<Timer>;
};

#endif
