#include "Sprite.h"

namespace minesweeper {
    Sprite::Sprite(ImageRepo &imageRepo, Renderer &renderer, SDL_Rect boundingBox)
            : imageRepo(imageRepo), renderer(renderer), boundingBox(boundingBox) {

    }

    bool Sprite::contains(int x, int y) {
        return x >= boundingBox.x
               && x < boundingBox.x + boundingBox.w
               && y >= boundingBox.y
               && y < boundingBox.y + boundingBox.h;
    }

    void Sprite::onClick(SDL_MouseButtonEvent evt) {
        if (contains(evt.x, evt.y)) {
            handleClick(evt);
        }
    }

    void Sprite::render() {

    }

    void Sprite::handleClick(SDL_MouseButtonEvent evt) {

    }
}