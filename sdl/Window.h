#ifndef MINESWEEPER_WINDOW_H
#define MINESWEEPER_WINDOW_H

#include "Renderer.h"

namespace minesweeper {
    class Window {
    public:
        Window(SDL_Rect rect);
        ~Window();
        Renderer createRenderer();
    private:
        SDL_Window *win;
    };
};

#endif
