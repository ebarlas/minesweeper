#ifndef MINESWEEPER_TEXTURE_H
#define MINESWEEPER_TEXTURE_H

#include <memory>
#include "SDL.h"

namespace minesweeper {
    class Texture {
    public:
        Texture();
        Texture(SDL_Renderer *ren, SDL_Texture *texture);
        ~Texture();
        void render(SDL_Rect *rect);
    private:
        SDL_Renderer *ren;
        SDL_Texture *texture;
    };

    using TexturePtr = std::shared_ptr<Texture>;
}

#endif
