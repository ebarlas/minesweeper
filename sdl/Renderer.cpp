#include "Renderer.h"

namespace minesweeper {
    Renderer::Renderer(SDL_Window *win) :
            ren(SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) {
        if (ren == nullptr) {
            ren = SDL_CreateRenderer(win, -1, 0);
        }
    }

    Renderer::~Renderer() {
        SDL_DestroyRenderer(ren);
    }

    ImageRepo Renderer::createImageRepo(const char *dir) {
        return ImageRepo{ren, dir};
    }

    void Renderer::repaint() {
        SDL_RenderPresent(ren);
    }
}
