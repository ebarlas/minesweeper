#include "Game.h"

#include "Background.h"
#include "Timer.h"
#include "FlagCounter.h"
#include "Button.h"
#include "Grid.h"

namespace minesweeper {
    Game::Game(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout, Mode::Enum mode)
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

    void Game::run() {
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

    void Game::onClick(SDL_MouseButtonEvent evt) {
        for (auto &sprite : sprites)
            sprite->onClick(evt);
    }

    void Game::render() {
        for (auto &sprite : sprites)
            sprite->render();
        renderer.repaint();
    }
}
