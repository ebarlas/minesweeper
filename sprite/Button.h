#ifndef MINESWEEPER_BUTTON_H
#define MINESWEEPER_BUTTON_H

#include <vector>
#include "../config/Options.h"
#include "../config/Layout.h"
#include "TileListener.h"
#include "Sprite.h"
#include "GameStateListener.h"

namespace minesweeper {
    class Button : public Sprite, public TileListener {
    public:
        Button(ImageRepo &imageRepo, const Options &options, const Layout &layout);
        void setListeners(const std::vector<GameStateListenerWPtr> &v);
        void handleClick(SDL_MouseButtonEvent evt) override;
        void onReveal(bool mine, bool adjacentMines) override;
        void render() override;
    private:
        GameState state;
        int revealed;
        const Options &options;
        std::vector<GameStateListenerWPtr> listeners;
        TexturePtr getFaceImage();
        void notifyListeners();
    };

    using ButtonPtr = std::shared_ptr<Button>;
};

#endif
