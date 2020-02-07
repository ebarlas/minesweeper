#include "Texture.h"

namespace minesweeper {
    Texture::Texture() : Texture(nullptr, nullptr) {

    }

    Texture::Texture(SDL_Renderer *ren, SDL_Texture *texture) : ren(ren), texture(texture) {

    }

    Texture::~Texture() {
        SDL_DestroyTexture(texture);
    }

    void Texture::render(SDL_Rect *rect) {
        SDL_RenderCopy(ren, texture, nullptr, rect);
    }
}
