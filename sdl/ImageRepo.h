#ifndef MINESWEEPER_IMAGEREPO_H
#define MINESWEEPER_IMAGEREPO_H

#include <string>
#include <map>
#include "Texture.h"
#include "../config/Mode.h"

namespace minesweeper {
    class ImageRepo {
    public:
        ImageRepo(SDL_Renderer *ren, const char *dir, const Mode::Enum mode);
        TexturePtr getBackground();
        TexturePtr getDigitPanel();
        TexturePtr getDigit(int digit);
        TexturePtr getFacePlaying();
        TexturePtr getFaceWin();
        TexturePtr getFaceLose();
        TexturePtr getTileFlag();
        TexturePtr getTileMine();
        TexturePtr getTile();
        TexturePtr getTile(int num);
    private:
        SDL_Renderer *ren;
        const char *dir;
        const Mode::Enum mode;
        std::map<std::string, TexturePtr> images;
        SDL_Texture *load(const char *name);
        TexturePtr get(const char *name);
    };
}

#endif
