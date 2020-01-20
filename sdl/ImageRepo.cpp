#include "ImageRepo.h"

namespace minesweeper {
    SDL_Texture *ImageRepo::load(const char *name) {
        std::string file{dir};
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

    ImageRepo::ImageRepo(SDL_Renderer *ren, const char *dir, const Mode::Enum mode) : ren(ren), dir(dir), mode(mode) {

    }

    TexturePtr ImageRepo::getBackground() {
        switch (mode) {
            case Mode::BEGINNER:
                return get("minesweeper_bg_beginner");
            case Mode::INTERMEDIATE:
                return get("minesweeper_bg_intermediate");
            default:
                return get("minesweeper_bg_expert");
        }
    }

    TexturePtr ImageRepo::getDigitPanel() {
        return get("minesweeper_digit_panel");
    }

    TexturePtr ImageRepo::getDigit(int digit) {
        switch (digit) {
            case 1:
                return get("minesweeper_digit_one");
            case 2:
                return get("minesweeper_digit_two");
            case 3:
                return get("minesweeper_digit_three");
            case 4:
                return get("minesweeper_digit_four");
            case 5:
                return get("minesweeper_digit_five");
            case 6:
                return get("minesweeper_digit_six");
            case 7:
                return get("minesweeper_digit_seven");
            case 8:
                return get("minesweeper_digit_eight");
            case 9:
                return get("minesweeper_digit_nine");
            default:
                return get("minesweeper_digit_zero");
        }
    }

    TexturePtr ImageRepo::getFacePlaying() {
        return get("minesweeper_face_playing");
    }

    TexturePtr ImageRepo::getFaceWin() {
        return get("minesweeper_face_win");
    }

    TexturePtr ImageRepo::getFaceLose() {
        return get("minesweeper_face_lose");
    }

    TexturePtr ImageRepo::getTileFlag() {
        return get("minesweeper_tile_flag");
    }

    TexturePtr ImageRepo::getTileMine() {
        return get("minesweeper_tile_mine");
    }

    TexturePtr ImageRepo::getTile() {
        return get("minesweeper_tile");
    }

    TexturePtr ImageRepo::getTile(int num) {
        switch (num) {
            case 1:
                return get("minesweeper_tile_one");
            case 2:
                return get("minesweeper_tile_two");
            case 3:
                return get("minesweeper_tile_three");
            case 4:
                return get("minesweeper_tile_four");
            case 5:
                return get("minesweeper_tile_five");
            case 6:
                return get("minesweeper_tile_six");
            case 7:
                return get("minesweeper_tile_seven");
            case 8:
                return get("minesweeper_tile_eight");
            default:
                return get("minesweeper_tile_none");
        }
    }
};