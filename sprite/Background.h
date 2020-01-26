#ifndef MINESWEEPER_BACKGROUND_H
#define MINESWEEPER_BACKGROUND_H

#include "../config/Layout.h"
#include "Sprite.h"

namespace minesweeper {
    class Background : public Sprite {
    public:
        Background(ImageRepo &imageRepo, const Layout &layout, Mode::Enum mode);
        void render() override;
    private:
        Mode::Enum mode;
        const char *getBackground();
    };

    using BackgroundPtr = std::shared_ptr<Background>;
};

#endif
