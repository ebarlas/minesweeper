#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

#include <vector>
#include "../sdl/ImageRepo.h"
#include "../sdl/Renderer.h"
#include "../config/Layout.h"
#include "../config/Options.h"
#include "Sprite.h"

namespace minesweeper {
    class Game {
    public:
        Game(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout, Mode::Enum mode);
        void run();
    private:
        Renderer &renderer;
        std::vector<SpritePtr> sprites;
        void onClick(SDL_MouseButtonEvent evt);
        void render();
    };
};

#endif
