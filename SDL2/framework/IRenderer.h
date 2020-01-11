#pragma once

#include "SDL.h"

#include <memory>

namespace sdl {

class IRenderer
{
public:
    using IImageRepoPtr = std::shared_ptr<class IImageRepo>;

public:
    virtual ~IRenderer() = default;

public:
    virtual IImageRepoPtr createImageRepo(const std::string &folder) = 0;
};
using IRendererPtr = std::shared_ptr<IRenderer>;

} // namespace sdl
