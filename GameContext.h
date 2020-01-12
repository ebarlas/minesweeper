#pragma once

#include "Layout.h"
#include "Mode.h"

#include "SDL2/framework/IWindow.h"
#include "SDL2/framework/IRenderer.h"
#include "SDL2/framework/IImageRepo.h"

namespace minesweeper {

class Context
{
public:
    sdl::IWindowPtr window;
    sdl::IRendererPtr renderer;
    sdl::IImageRepoPtr imageRepo;
    Layout layout;
    Mode::Enum mode;
};

using ContextPtr = std::shared_ptr<Context>;

} // namespace minesweeper
