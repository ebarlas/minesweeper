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
#include "sdl/Texture.h"

using namespace minesweeper;

class ImageRepo {
private:
    SDL_Renderer *ren;
    const char *dir;
    const Mode::Enum mode;

    std::map<std::string, TexturePtr> images;

    SDL_Texture *load(const char *name) {
        std::string file{dir};
        file.append(name);
        file.append(".bmp");
        SDL_Surface *image = SDL_LoadBMP(file.c_str());
        SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);
        SDL_FreeSurface(image);
        return texture;
    }

    TexturePtr get(const char *name) {
        auto it = images.find(name);
        return it != images.end() ? it->second : images[name] = std::make_shared<Texture>(ren, load(name));
    }

public:
    ImageRepo(SDL_Renderer *ren, const char *dir, const Mode::Enum mode) : ren(ren), dir(dir), mode(mode) {

    }

    TexturePtr getBackground() {
        switch (mode) {
            case Mode::BEGINNER:
                return get("minesweeper_bg_beginner");
            case Mode::INTERMEDIATE:
                return get("minesweeper_bg_intermediate");
            default:
                return get("minesweeper_bg_expert");
        }
    }

    TexturePtr getDigitPanel() {
        return get("minesweeper_digit_panel");
    }

    TexturePtr getDigit(int digit) {
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

    TexturePtr getFacePlaying() {
        return get("minesweeper_face_playing");
    }

    TexturePtr getFaceWin() {
        return get("minesweeper_face_win");
    }

    TexturePtr getFaceLose() {
        return get("minesweeper_face_lose");
    }

    TexturePtr getTileFlag() {
        return get("minesweeper_tile_flag");
    }

    TexturePtr getTileMine() {
        return get("minesweeper_tile_mine");
    }

    TexturePtr getTile() {
        return get("minesweeper_tile");
    }

    TexturePtr getTile(int num) {
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

class Renderer {
private:
    SDL_Renderer *ren;
    const Mode::Enum mode;
public:
    explicit Renderer(SDL_Window *win, const Mode::Enum mode) :
            ren(SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)),
            mode(mode) {
        if (ren == nullptr) {
            ren = SDL_CreateRenderer(win, -1, 0);
        }
    }

    ~Renderer() {
        SDL_DestroyRenderer(ren);
    }

    ImageRepo createImageRepo(const char *dir) {
        return ImageRepo{ren, dir, mode};
    }

    void repaint() {
        SDL_RenderPresent(ren);
    }
};

class Sprite {
protected:
    ImageRepo &imageRepo;
    Renderer &renderer;

    SDL_Rect boundingBox;

    bool contains(int x, int y) {
        return x >= boundingBox.x
               && x < boundingBox.x + boundingBox.w
               && y >= boundingBox.y
               && y < boundingBox.y + boundingBox.h;
    }

    Sprite(ImageRepo &imageRepo, Renderer &renderer, SDL_Rect boundingBox)
            : imageRepo(imageRepo), renderer(renderer), boundingBox(boundingBox) {

    }

public:
    void onClick(SDL_MouseButtonEvent evt) {
        if (contains(evt.x, evt.y)) {
            handleClick(evt);
        }
    }

    virtual void render() {

    }

    virtual void handleClick(SDL_MouseButtonEvent evt) {

    }

    virtual ~Sprite() = default;
};

using SpritePtr = std::shared_ptr<Sprite>;

class DigitPanel : public Sprite {
protected:
    DigitPanel(ImageRepo &imageRepo, Renderer &renderer, SDL_Rect boundingBox)
            : Sprite(imageRepo, renderer, boundingBox) {

    }

    virtual SDL_Rect getDigitRect(int position) = 0;

    virtual int getDisplayValue() = 0;

public:
    void render() override {
        imageRepo.getDigitPanel()->render(&boundingBox);

        int value = getDisplayValue();
        int onesDigit = value % 10;
        int tensDigit = (value / 10) % 10;
        int hundredsDigit = (value / 100) % 10;

        SDL_Rect rect = getDigitRect(0);
        imageRepo.getDigit(hundredsDigit)->render(&rect);

        rect = getDigitRect(1);
        imageRepo.getDigit(tensDigit)->render(&rect);

        rect = getDigitRect(2);
        imageRepo.getDigit(onesDigit)->render(&rect);
    }
};

enum class GameState {
    INIT,
    PLAYING,
    WON,
    LOST
};

class GameStateListener {
public:
    virtual void onStateChange(GameState state) = 0;

    virtual ~GameStateListener() = default;
};

using GameStateListenerPtr = std::shared_ptr<GameStateListener>;
using GameStateListenerWPtr = std::weak_ptr<GameStateListener>;

class Timer : public DigitPanel, public GameStateListener {
private:
    const Layout &layout;
    ClockTimer timer;
    bool running;
    int elapsed;
public:
    Timer(ImageRepo &imageRepo, Renderer &renderer, const Layout &layout)
            : DigitPanel(imageRepo, renderer, layout.getTimerDigitPanel()), layout(layout), running(false), elapsed(0) {

    }

    SDL_Rect getDigitRect(int position) override {
        return layout.getTimerDigit(position);
    }

    int getDisplayValue() override {
        return running ? static_cast<int>(timer.elapsed()) : elapsed;
    }

    void onStateChange(GameState state) override {
        if (state == GameState::PLAYING) {
            running = true;
            timer.reset();
        } else if (state == GameState::WON || state == GameState::LOST) {
            running = false;
            elapsed = static_cast<int>(timer.elapsed());
        } else {
            running = false;
            elapsed = 0;
        }
    }
};

using TimerPtr = std::shared_ptr<Timer>;

class TileListener {
public:
    virtual void onReveal(bool mine, bool adjacentMines) {

    }

    virtual void onClear() {

    }

    virtual void onFlag(bool flagged) {

    }
};

using TileListenerPtr = std::shared_ptr<TileListener>;
using TileListenerWPtr = std::weak_ptr<TileListener>;

class FlagStateListener {
public:
    virtual void onFlagStateChange(bool exhausted) = 0;

    virtual ~FlagStateListener() = default;
};

using FlagStateListenerPtr = std::shared_ptr<FlagStateListener>;
using FlagStateListenerWPtr = std::weak_ptr<FlagStateListener>;

class FlagCounter : public DigitPanel, public GameStateListener, public TileListener {
private:
    const Options &options;
    const Layout &layout;
    int flags;

    std::vector<FlagStateListenerWPtr> listeners;

    void notifyListeners(bool exhausted) {
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onFlagStateChange(exhausted);
    }

public:
    FlagCounter(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout) :
            DigitPanel(imageRepo, renderer, layout.getFlagsDigitPanel()),
            options(options),
            layout(layout),
            flags(options.getMines()) {

    }

    void setListeners(const std::vector<FlagStateListenerWPtr> &v) {
        listeners = v;
    }

    SDL_Rect getDigitRect(int position) override {
        return layout.getFlagsDigit(position);
    }

    int getDisplayValue() override {
        return flags;
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT)
            flags = options.getMines();
    }

    void onFlag(bool flagged) override {
        if (flagged) {
            flags--;
            if (flags == 0)
                notifyListeners(true);
        } else {
            flags++;
            if (flags == 1)
                notifyListeners(false);
        }
    }
};

using FlagCounterPtr = std::shared_ptr<FlagCounter>;

class Button : public Sprite, public TileListener {
private:
    GameState state;
    int revealed;
    const Options &options;

    std::vector<GameStateListenerWPtr> listeners;

    TexturePtr getFaceImage() {
        switch (state) {
            case GameState::INIT:
            case GameState::PLAYING:
                return imageRepo.getFacePlaying();
            case GameState::WON:
                return imageRepo.getFaceWin();
            default:
                return imageRepo.getFaceLose();
        }
    }

    void notifyListeners() {
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onStateChange(state);
    }

public:
    Button(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout) :
            Sprite(imageRepo, renderer, layout.getFace()),
            state(GameState::INIT),
            revealed(0),
            options(options) {

    }

    void setListeners(const std::vector<GameStateListenerWPtr> &v) {
        listeners = v;
    }

    void handleClick(SDL_MouseButtonEvent evt) override {
        state = GameState::INIT;
        revealed = 0;
        notifyListeners();
    }

    void onReveal(bool mine, bool adjacentMines) override {
        if (mine) {
            state = GameState::LOST;
            notifyListeners();
        } else {
            if (state == GameState::INIT) {
                state = GameState::PLAYING;
                notifyListeners();
            }
            revealed++;
            if (revealed == options.getBlanks()) {
                state = GameState::WON;
                notifyListeners();
            }
        }
    }

    void render() override {
        getFaceImage()->render(&boundingBox);
    }
};

using ButtonPtr = std::shared_ptr<Button>;

class MineField {
private:
    Random random;
    std::set<int> mines;
    const Options &options;
public:
    explicit MineField(const Options &options) : options(options) {
        reset();
    }

    void reset() {
        mines.clear();
        for (int i = 1; i <= options.getMines(); i++) {
            while (mines.size() < i) {
                mines.insert(random.randomInt(0, options.getTiles() - 1));
            }
        }
    }

    bool mineAt(int row, int col) {
        int projection = row * options.getColumns() + col;
        return mines.find(projection) != mines.end();
    }

    int adjacentMines(int row, int col) {
        int sum = 0;
        auto fn = [&sum, this](int r, int c) { sum = sum + (mineAt(r, c) ? 1 : 0); };
        options.forEachNeighbor(row, col, fn);
        return sum;
    }
};

class Tile : public Sprite, public TileListener, public GameStateListener, public FlagStateListener {
private:
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
    Tile(ImageRepo &repo, Renderer &ren, const SDL_Rect boundingBox, int adjMines, bool mine) :
            Sprite(repo, ren, boundingBox),
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
                imageRepo.getTileMine()->render(&boundingBox);
            } else {
                imageRepo.getTile(adjacentMines)->render(&boundingBox);
            }
        } else if (flagged) {
            imageRepo.getTileFlag()->render(&boundingBox);
        } else {
            imageRepo.getTile()->render(&boundingBox);
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
    Grid(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout) :
            Sprite(imageRepo, renderer, layout.getGrid()),
            tiles{options.getRows(), options.getColumns(), TilePtr()},
            mineField(options),
            options(options) {
        auto fn = [&imageRepo, &renderer, &layout, this](int r, int c, TilePtr &t) {
            int adjacentMines = mineField.adjacentMines(r, c);
            bool mine = mineField.mineAt(r, c);
            SDL_Rect rect = layout.getTile(boundingBox.x, boundingBox.y, r, c);
            t = std::make_shared<Tile>(imageRepo, renderer, rect, adjacentMines, mine);
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
    Background(ImageRepo &imageRepo, Renderer &renderer, const Layout &layout)
            : Sprite(imageRepo, renderer, layout.getBackground()) {

    }

    void render() override {
        imageRepo.getBackground()->render(&boundingBox);
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
    Game(ImageRepo &imageRepo, Renderer &renderer, const Options &options, const Layout &layout) : renderer(renderer) {
        BackgroundPtr background{std::make_shared<Background>(imageRepo, renderer, layout)};
        TimerPtr timer{std::make_shared<Timer>(imageRepo, renderer, layout)};
        FlagCounterPtr flagCounter{std::make_shared<FlagCounter>(imageRepo, renderer, options, layout)};
        ButtonPtr button{std::make_shared<Button>(imageRepo, renderer, options, layout)};
        GridPtr grid{std::make_shared<Grid>(imageRepo, renderer, options, layout)};

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
    const Mode::Enum mode;
public:
    Window(const Layout &layout, const Mode::Enum mode) : mode(mode) {
        SDL_Rect rect = layout.getWindow();
        win = SDL_CreateWindow("Minesweeper", rect.x, rect.y, rect.w, rect.h, SDL_WINDOW_SHOWN);
    }

    ~Window() {
        SDL_DestroyWindow(win);
    }

    Renderer createRenderer() {
        return Renderer{win, mode};
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

    Window window{layout, mode};
    Renderer renderer{window.createRenderer()};
    ImageRepo imageRepo{renderer.createImageRepo("images/")};

    Game game{imageRepo, renderer, options, layout};
    game.run();

    SDL_Quit();
}
