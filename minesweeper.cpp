#include <iostream>
#include <vector>
#include <memory>
#include "SDL.h"
#include "config/Mode.h"
#include "config/Options.h"
#include "config/Layout.h"
#include "util/ClockTimer.h"
#include "util/Random.h"
#include "util/Matrix.h"
#include "sdl/ImageRepo.h"
#include "sdl/Renderer.h"
#include "sprite/Sprite.h"
#include "sprite/GameStateListener.h"
#include "sprite/Timer.h"
#include "sprite/TileListener.h"
#include "sprite/FlagStateListener.h"
#include "sprite/FlagCounter.h"
#include "sprite/Button.h"
#include "sprite/Grid.h"

using namespace minesweeper;

class Background : public Sprite {
public:
    Background(ImageRepo &imageRepo, const Layout &layout, Mode::Enum mode)
            : Sprite(imageRepo, layout.getBackground()), mode(mode) {

    }

    void render() override {
        imageRepo.get(getBackground())->render(&boundingBox);
    }

private:
    Mode::Enum mode;

    const char *getBackground() {
        switch (mode) {
            case Mode::BEGINNER:
                return "bg_beginner";
            case Mode::INTERMEDIATE:
                return "bg_intermediate";
            default:
                return "bg_expert";
        }
    }
};

using BackgroundPtr = std::shared_ptr<Background>;

class Game {
private:
    Renderer &renderer;
    std::vector<SpritePtr> sprites;

    void onClick(SDL_MouseButtonEvent evt) {
        for (auto &sprite : sprites)
            sprite->onClick(evt);
    }

    void render() {
        for (auto &sprite : sprites)
            sprite->render();
        renderer.repaint();
    }

public:
    Game(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout, Mode::Enum mode)
            : renderer(renderer) {
        BackgroundPtr background{std::make_shared<Background>(imageRepo, layout, mode)};
        TimerPtr timer{std::make_shared<Timer>(imageRepo, layout)};
        FlagCounterPtr flagCounter{std::make_shared<FlagCounter>(imageRepo, options, layout)};
        ButtonPtr button{std::make_shared<Button>(imageRepo, options, layout)};
        GridPtr grid{std::make_shared<Grid>(imageRepo, options, layout)};

        std::vector<GameStateListenerWPtr> gameStateListeners{grid, timer, flagCounter};
        button->setListeners(gameStateListeners);

        std::vector<TileListenerWPtr> tileRevealListeners{button, flagCounter};
        grid->setListeners(tileRevealListeners);

        std::vector<FlagStateListenerWPtr> flagStateListeners{grid};
        flagCounter->setListeners(flagStateListeners);

        sprites.push_back(background);
        sprites.push_back(timer);
        sprites.push_back(flagCounter);
        sprites.push_back(button);
        sprites.push_back(grid);
    }

    void run() {
        render();
        while (true) {
            SDL_Event e;
            int res = SDL_WaitEventTimeout(&e, 100);
            if (res != 0) {
                if (e.type == SDL_QUIT) {
                    break;
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    onClick(e.button);
                    render();
                }
            } else {
                // render on timeout
                render();
            }
        }
    }
};

class Window {
private:
    SDL_Window *win;
public:
    Window(const Layout &layout) {
        SDL_Rect rect = layout.getWindow();
        win = SDL_CreateWindow("Minesweeper", rect.x, rect.y, rect.w, rect.h, SDL_WINDOW_SHOWN);
    }

    ~Window() {
        SDL_DestroyWindow(win);
    }

    Renderer createRenderer() {
        return Renderer{win};
    }
};

int main(int argc, char **argv) {
    Mode::Enum mode = Mode::parse(argc > 1 ? *argv[1] : 'e');
    Options options{Options::getOptions(mode)};
    Layout layout{mode};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    Window window{layout};
    Renderer renderer{window.createRenderer()};
    ImageRepo imageRepo{renderer.createImageRepo("images/")};

    Game game{imageRepo, renderer, options, layout, mode};
    game.run();

    SDL_Quit();
    return 0;
}
