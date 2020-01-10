#include "Window.h"
#include "Renderer.h"

namespace sdl {

IWindowPtr IWindow::create(const std::string &name, const SDL_Rect &frame, unsigned flags)
{
    return std::make_shared<Window>(name, frame, flags);
}

Window::Window(const std::string &name, const SDL_Rect &frame, unsigned flags)
{
    auto win = SDL_CreateWindow(name.c_str(), frame.x, frame.y, frame.w, frame.h, flags);
    swap(win);
}

IRendererPtr Window::createRenderer() const
{
    constexpr unsigned flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    auto r = SDL_CreateRenderer(get(), -1, flags);
    if (r == nullptr)
        r = SDL_CreateRenderer(get(), -1, 0);
    return std::make_shared<Renderer>(r);
}

} // namespace sdl
