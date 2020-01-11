#pragma once

#include "SDL.h"

#include <string>
#include <memory>

namespace sdl {

class IImageRepo
{
public:
    using ITexturePtr = std::shared_ptr<class ITexture>;

public:
    virtual ~IImageRepo() = default;

public:
    virtual ITexturePtr get(const std::string &name) = 0;
};
using IImageRepoPtr = std::shared_ptr<IImageRepo>;

} // namespace sdl
