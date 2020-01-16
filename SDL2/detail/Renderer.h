#pragma once

#include "../framework/IRenderer.h"
#include "SDLWrapper.h"

namespace sdl {

class Renderer : public IRenderer,
                 public Wrapper<SDL_Renderer>,
                 public std::enable_shared_from_this<Renderer>
{
public:
    explicit Renderer(SDL_Renderer *renderer) : Wrapper<SDL_Renderer>(renderer) {}

public:
    IImageRepoPtr createImageRepo(const std::string &folder) override;
    void repaint() override;
};

using RendererWPtr = std::weak_ptr<Renderer>;

} // namespace sdl
