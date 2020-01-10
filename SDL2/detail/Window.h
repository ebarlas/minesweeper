#pragma once
#include "../framework/IWindow.h"
#include "SDLWrapper.h"

namespace sdl {

class Window : public IWindow, protected Wrapper<SDL_Window>
{
public:
    explicit Window(SDL_Window *window) : Wrapper<SDL_Window>(window) {}
    Window(const std::string &name, const SDL_Rect &frame, unsigned flags);

    IRendererPtr createRenderer() const override;
};

} // namespace sdl
