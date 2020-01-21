#include "ImageRepo.h"

namespace minesweeper {
    SDL_Texture *ImageRepo::load(const char *name) {
        std::string file{dir};
        file.append("minesweeper_");
        file.append(name);
        file.append(".bmp");
        SDL_Surface *image = SDL_LoadBMP(file.c_str());
        SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);
        SDL_FreeSurface(image);
        return texture;
    }

    TexturePtr ImageRepo::get(const char *name) {
        auto it = images.find(name);
        return it != images.end() ? it->second : images[name] = std::make_shared<Texture>(ren, load(name));
    }

    ImageRepo::ImageRepo(SDL_Renderer *ren, const char *dir) : ren(ren), dir(dir) {

    }
};