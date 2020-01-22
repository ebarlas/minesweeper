#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <memory>
#include "SDL.h"
#include "config/Mode.h"
#include "config/Options.h"
#include "config/Layout.h"
#include "util/ClockTimer.h"
#include "util/Random.h"
#include "sdl/ImageRepo.h"
#include "sdl/Renderer.h"
#include "sprite/Sprite.h"
#include "sprite/GameStateListener.h"
#include "sprite/Timer.h"
#include "sprite/TileListener.h"
#include "sprite/FlagStateListener.h"
#include "sprite/FlagCounter.h"
#include "sprite/Button.h"
#include "sprite/MineField.h"

using namespace minesweeper;

class Tile : public Sprite, public TileListener, public GameStateListener, public FlagStateListener {
private:
    static constexpr const char *TILES[]{"tile_none", "tile_one", "tile_two", "tile_three", "tile_four",
                                         "tile_five", "tile_six", "tile_seven", "tile_eight"};

    int adjacentMines;
    int adjacentFlags;
    bool mine;
    bool flagged;
    bool revealed;
    bool gameOver;
    bool flagRemaining;

    std::vector<TileListenerWPtr> listeners;

    void tryReveal() {
        if (gameOver || flagged || revealed)
            return;
        revealed = true;
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onReveal(mine, adjacentMines > 0);
    }

    void tryToggleFlag() {
        if (gameOver || revealed)
            return;
        if (!flagged && !flagRemaining)
            return;
        flagged = !flagged;
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onFlag(flagged);
    }

    void tryClear() {
        if (adjacentFlags == adjacentMines)
            for (auto &listener : listeners)
                if (auto spt = listener.lock())
                    spt->onClear();
    }

public:
    Tile(ImageRepo &repo, const SDL_Rect boundingBox, int adjMines, bool mine) :
            Sprite(repo, boundingBox),
            adjacentMines(adjMines),
            adjacentFlags(0),
            mine(mine),
            flagged(false),
            revealed(false),
            gameOver(false),
            flagRemaining(true) {

    }

    void setListeners(const std::vector<TileListenerWPtr> &v) {
        listeners = v;
    }

    void reset(int adjMines, bool myMine) {
        adjacentMines = adjMines;
        mine = myMine;
    }

    void onReveal(bool hasMine, bool hasAdjacentMines) override {
        if (!hasMine && !hasAdjacentMines) {
            tryReveal();
        }
    }

    void onFlag(bool isFlagged) override {
        if (isFlagged) {
            adjacentFlags++;
        } else {
            adjacentFlags--;
        }
    }

    void onClear() override {
        tryReveal();
    }

    void handleClick(SDL_MouseButtonEvent evt) override {
        if (evt.button == SDL_BUTTON_LEFT) {
            if (revealed) {
                tryClear();
            } else {
                tryReveal();
            }
        } else if (evt.button == SDL_BUTTON_RIGHT) {
            tryToggleFlag();
        }
    }

    void onStateChange(GameState gs) override {
        if (gs == GameState::INIT) {
            flagged = false;
            revealed = false;
            adjacentFlags = 0;
            gameOver = false;
            flagRemaining = true;
        }
        if (gs == GameState::WON || gs == GameState::LOST) {
            gameOver = true;
        }
    }

    void onFlagStateChange(bool exhausted) override {
        flagRemaining = !exhausted;
    }

    void render() override {
        if (revealed) {
            if (mine) {
                imageRepo.get("tile_mine")->render(&boundingBox);
            } else {
                imageRepo.get(TILES[adjacentMines])->render(&boundingBox);
            }
        } else if (flagged) {
            imageRepo.get("tile_flag")->render(&boundingBox);
        } else {
            imageRepo.get("tile")->render(&boundingBox);
        }
    }
};

using TilePtr = std::shared_ptr<Tile>;
using TileWPtr = std::weak_ptr<Tile>;

template<typename T>
class Matrix {
private:
    int rows;
    int columns;
    std::vector<T> matrix;
public:
    Matrix(int rows, int columns, const T &value) : rows(rows), columns(columns), matrix(rows * columns, value) {

    }

    T &at(int row, int col) {
        int n = row * columns + col;
        return matrix[n];
    }

    void forEach(std::function<void(int row, int col, T &val)> fn) {
        for (int i = 0; i < matrix.size(); i++) {
            int row = i / columns;
            int col = i % columns;
            fn(row, col, matrix[i]);
        }
    }
};

class Grid : public Sprite, public GameStateListener, public FlagStateListener {
private:
    Matrix<TilePtr> tiles;
    MineField mineField;
    const Options &options;
public:
    Grid(ImageRepo &imageRepo, const Options &options, const Layout &layout) :
            Sprite(imageRepo, layout.getGrid()),
            tiles{options.getRows(), options.getColumns(), TilePtr()},
            mineField(options),
            options(options) {
        auto fn = [&imageRepo, &layout, this](int r, int c, TilePtr &t) {
            int adjacentMines = mineField.adjacentMines(r, c);
            bool mine = mineField.mineAt(r, c);
            SDL_Rect rect = layout.getTile(boundingBox.x, boundingBox.y, r, c);
            t = std::make_shared<Tile>(imageRepo, rect, adjacentMines, mine);
        };
        tiles.forEach(fn);
    }

    void setListeners(const std::vector<TileListenerWPtr> &v) {
        auto fe = [&v, this](int r, int c, TilePtr &t) {
            std::vector<TileListenerWPtr> listeners = v;
            auto fn = [&listeners, this](int r, int c) { listeners.push_back(tiles.at(r, c)); };
            options.forEachNeighbor(r, c, fn);
            t->setListeners(listeners);
        };
        tiles.forEach(fe);
    }

    void handleClick(SDL_MouseButtonEvent evt) override {
        int col = (evt.x - boundingBox.x) / Layout::getTileSide();
        int row = (evt.y - boundingBox.y) / Layout::getTileSide();
        tiles.at(row, col)->handleClick(evt);
    }

    void onFlagStateChange(bool exhausted) override {
        tiles.forEach([exhausted](int r, int c, std::shared_ptr<Tile> &t) { t->onFlagStateChange(exhausted); });
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT) {
            mineField.reset();
            auto fn = [this](int r, int c, TilePtr &t) {
                t->reset(mineField.adjacentMines(r, c), mineField.mineAt(r, c));
            };
            tiles.forEach(fn);
        }

        tiles.forEach([state](int r, int c, std::shared_ptr<Tile> &t) { t->onStateChange(state); });
    }

    void render() override {
        tiles.forEach([](int r, int c, TilePtr &t) { t->render(); });
    }
};

using GridPtr = std::shared_ptr<Grid>;

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
