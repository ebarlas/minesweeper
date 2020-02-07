#include "FlagCounter.h"

namespace minesweeper {
    FlagCounter::FlagCounter(ImageRepo &imageRepo, const Options &options, const Layout &layout) :
            DigitPanel(imageRepo, layout.getFlagsDigitPanel()),
            options(options),
            layout(layout),
            flags(options.getMines()) {

    }

    void FlagCounter::setListeners(const std::vector<FlagStateListenerWPtr> &v) {
        listeners = v;
    }

    SDL_Rect FlagCounter::getDigitRect(int position) {
        return layout.getFlagsDigit(position);
    }

    int FlagCounter::getDisplayValue() {
        return flags;
    }

    void FlagCounter::onStateChange(GameState state) {
        if (state == GameState::INIT)
            flags = options.getMines();
    }

    void FlagCounter::onFlag(bool flagged) {
        if (flagged) {
            flags--;
            if (flags == 0)
                notifyListeners(true);
        } else {
            flags++;
            if (flags == 1)
                notifyListeners(false);
        }
    }

    void FlagCounter::notifyListeners(bool exhausted) {
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onFlagStateChange(exhausted);
    }
}
