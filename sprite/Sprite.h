#ifndef MINESWEEPER_SPRITE_H
#define MINESWEEPER_SPRITE_H

#include "SDL.h"
#include "../sdl/Renderer.h"

namespace minesweeper {
    class Sprite {
    public:
        void onClick(SDL_MouseButtonEvent evt) ;
        virtual void render();
        virtual void handleClick(SDL_MouseButtonEvent evt);
        virtual ~Sprite() = default;
    protected:
        ImageRepo &imageRepo;
        Renderer &renderer;
        SDL_Rect boundingBox;
        bool contains(int x, int y);
        Sprite(ImageRepo &imageRepo, Renderer &renderer, SDL_Rect boundingBox);
    };

    using SpritePtr = std::shared_ptr<Sprite>;
};

#endif
