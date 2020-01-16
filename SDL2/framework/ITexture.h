#pragma once

#include "SDL.h"

#include <memory>

namespace sdl {

class ITexture
{
public:
    virtual ~ITexture() = default;

public:
    virtual void render(const SDL_Rect &rect) const = 0;
};

using ITexturePtr = std::shared_ptr<ITexture>;
using ITextureWPtr = std::weak_ptr<ITexture>;

} // namespace sdl
