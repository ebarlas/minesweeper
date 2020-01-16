#pragma once

#include "SDL.h"

#include <string>
#include <memory>

namespace sdl {

class IWindow
{
public:
    using IWindowPtr = std::shared_ptr<IWindow>;
    using IRendererPtr = std::shared_ptr<class IRenderer>;

public:
    // factory
    static IWindowPtr create(const std::string &name, const SDL_Rect &frame, unsigned flags);

public:
    virtual ~IWindow() = default;

public:
    virtual IRendererPtr createRenderer() const = 0;
};
using IWindowPtr = IWindow::IWindowPtr;

} // namespace sdl
