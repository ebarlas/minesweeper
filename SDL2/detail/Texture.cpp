#include "Texture.h"
#include "Renderer.h"

namespace sdl {

void Texture::render(const SDL_Rect &rect) const
{
    if(auto renderer{_renderer.lock()})
        SDL_RenderCopy(renderer->getsdl(), getsdl(), nullptr, &rect);
}

} // namespace sdl
