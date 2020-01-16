#pragma once

#include "GameContext.h"

#include "SDL.h"

namespace minesweeper {

class Sprite
{
public:
    Sprite(const ContextPtr &context, const SDL_Rect &boundingBox)
        : _context(context), _boundingBox(boundingBox)
    {}

    virtual ~Sprite() = default;

public:
    void onClick(const SDL_MouseButtonEvent &evt)
    {
        if (contains(evt.x, evt.y)) {
            handleClick(evt);
        }
    }

    virtual void render() {}

    virtual void handleClick(const SDL_MouseButtonEvent &evt) {}

protected:
    bool contains(int x, int y) const
    {
        auto pt{SDL_Point{x, y}};
        return SDL_PointInRect(&pt, &_boundingBox);
    }

protected:
    const ContextPtr _context;
    const SDL_Rect _boundingBox;
};

using SpritePtr = std::shared_ptr<Sprite>;

} // namespace minesweeper
