#ifndef MINESWEEPER_SPRITE_H
#define MINESWEEPER_SPRITE_H

#include "../sdl/ImageRepo.h"

namespace minesweeper {
    class Sprite {
    public:
        void onClick(SDL_MouseButtonEvent evt) ;
        virtual void render();
        virtual void handleClick(SDL_MouseButtonEvent evt);
        virtual ~Sprite() = default;
    protected:
        ImageRepo &imageRepo;
        SDL_Rect boundingBox;
        bool contains(int x, int y);
        Sprite(ImageRepo &imageRepo, SDL_Rect boundingBox);
    };

    using SpritePtr = std::shared_ptr<Sprite>;
};

#endif
