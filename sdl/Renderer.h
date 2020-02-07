#ifndef MINESWEEPER_RENDERER_H
#define MINESWEEPER_RENDERER_H

#include "SDL.h"
#include "ImageRepo.h"

namespace minesweeper {
    class Renderer {
    public:
        explicit Renderer(SDL_Window *win);
        ~Renderer();
        ImageRepo createImageRepo(const char *dir);
        void repaint();
    private:
        SDL_Renderer *ren;
    };
};

#endif
