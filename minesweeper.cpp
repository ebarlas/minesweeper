#include <iostream>
#include "SDL.h"
#include "config/Mode.h"
#include "config/Options.h"
#include "config/Layout.h"
#include "sdl/ImageRepo.h"
#include "sdl/Renderer.h"
#include "sdl/Window.h"
#include "sprite/Game.h"

using namespace minesweeper;

int main(int argc, char **argv) {
    Mode::Enum mode = Mode::parse(argc > 1 ? *argv[1] : 'e');
    Options options{Options::getOptions(mode)};
    Layout layout{mode};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    Window window{layout.getWindow()};
    Renderer renderer{window.createRenderer()};
    ImageRepo imageRepo{renderer.createImageRepo("images/")};

    Game game{imageRepo, renderer, options, layout, mode};
    game.run();

    SDL_Quit();
    return 0;
}
