#pragma once

#include "../framework/ITexture.h"

#include "SDLWrapper.h"

namespace sdl {

class Texture : public ITexture, public Wrapper<SDL_Texture>
{
public:
    using RendererWPtr = std::weak_ptr<class Renderer>;

public:
    explicit Texture(SDL_Texture *texture) : Wrapper<SDL_Texture>(texture) {}
    Texture(SDL_Texture *texture, const RendererWPtr& renderer)
        : Wrapper<SDL_Texture>(texture), _renderer(renderer){}

public:
    void render(const SDL_Rect &rect) const override;

private:
    RendererWPtr _renderer;
};

} // namespace sdl
