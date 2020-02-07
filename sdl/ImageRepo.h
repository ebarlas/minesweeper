#ifndef MINESWEEPER_IMAGEREPO_H
#define MINESWEEPER_IMAGEREPO_H

#include <string>
#include <map>
#include "Texture.h"

namespace minesweeper {
    class ImageRepo {
    public:
        ImageRepo(SDL_Renderer *ren, const char *dir);
        TexturePtr get(const char *name);
    private:
        SDL_Renderer *ren;
        const char *dir;
        std::map<std::string, TexturePtr> images;
        SDL_Texture *load(const char *name);
    };
}

#endif
