#ifndef MINESWEEPER_FLAGCOUNTER_H
#define MINESWEEPER_FLAGCOUNTER_H

#include <vector>
#include "../config/Options.h"
#include "../config/Layout.h"
#include "DigitPanel.h"
#include "GameStateListener.h"
#include "TileListener.h"
#include "FlagStateListener.h"

namespace minesweeper {
    class FlagCounter : public DigitPanel, public GameStateListener, public TileListener {
    public:
        FlagCounter(ImageRepo &imageRepo, const Options &options, const Layout &layout);
        void setListeners(const std::vector<FlagStateListenerWPtr> &v);
        SDL_Rect getDigitRect(int position) override;
        int getDisplayValue() override;
        void onStateChange(GameState state) override;
        void onFlag(bool flagged) override;
    private:
        const Options &options;
        const Layout &layout;
        int flags;
        std::vector<FlagStateListenerWPtr> listeners;
        void notifyListeners(bool exhausted);
    };

    using FlagCounterPtr = std::shared_ptr<FlagCounter>;
};

#endif
