#include "Background.h"

namespace minesweeper {
    Background::Background(ImageRepo &imageRepo, const Layout &layout, Mode::Enum mode)
            : Sprite(imageRepo, layout.getBackground()), mode(mode) {

    }

    const char *Background::getBackground() {
        switch (mode) {
            case Mode::BEGINNER:
                return "bg_beginner";
            case Mode::INTERMEDIATE:
                return "bg_intermediate";
            default:
                return "bg_expert";
        }
    }

    void Background::render() {
        imageRepo.get(getBackground())->render(&boundingBox);
    }
}
