#pragma once
#include "../framework/IRenderer.h"
#include "SDLWrapper.h"
namespace sdl {

class Renderer : public IRenderer, protected Wrapper<SDL_Renderer>
{
public:
    explicit Renderer(SDL_Renderer *renderer) : Wrapper<SDL_Renderer>(renderer) {}
};

} // namespace sdl
