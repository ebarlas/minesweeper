#include "Window.h"

namespace minesweeper {
    Window::Window(SDL_Rect rect) {
        win = SDL_CreateWindow("Minesweeper", rect.x, rect.y, rect.w, rect.h, SDL_WINDOW_SHOWN);
    }

    Window::~Window() {
        SDL_DestroyWindow(win);
    }

    Renderer Window::createRenderer() {
        return Renderer{win};
    }
}
