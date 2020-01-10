#pragma once

#include "SDL.h"

#include <string>
#include <memory>

namespace sdl {

class IRenderer
{
public:
};
using IRendererPtr = std::shared_ptr<IRenderer>;

} // namespace sdl
