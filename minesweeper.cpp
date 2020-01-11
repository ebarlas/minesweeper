#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <random>
#include <set>
#include <functional>
#include <memory>
#include "SDL.h"

#include "SDL2/framework/IWindow.h"
#include "SDL2/framework/IRenderer.h"
#include "SDL2/framework/IImageRepo.h"
#include "SDL2/framework/ITexture.h"

class Mode {
public:
    enum Enum {
        BEGINNER,
        INTERMEDIATE,
        EXPERT
    };

    static Mode::Enum parse(const char mode) {
        switch (mode) {
            case 'b':
                return BEGINNER;
            case 'i':
                return INTERMEDIATE;
            default:
                return EXPERT;
        }
    }
};

class Options {
private:
    const int rows;
    const int columns;
    const int mines;
public:
    Options(int rows, int columns, int mines) : rows(rows), columns(columns), mines(mines) {

    }

    [[nodiscard]] int getTiles() const {
        return rows * columns;
    }

    [[nodiscard]] int getMines() const {
        return mines;
    }

    [[nodiscard]] int getBlanks() const {
        return getTiles() - mines;
    }

    [[nodiscard]] int getRows() const {
        return rows;
    }

    [[nodiscard]] int getColumns() const {
        return columns;
    }

    void forEachNeighbor(int row, int col, const std::function<void(int, int)> &fn) const {
        for (int r = row - 1; r <= row + 1; r++) {
            for (int c = col - 1; c <= col + 1; c++) {
                if (r != row || c != col) {
                    if (r >= 0 && r < rows && c >= 0 && c < columns) {
                        fn(r, c);
                    }
                }
            }
        }
    }

    static Options getOptions(const Mode::Enum mode) {
        switch (mode) {
            case Mode::BEGINNER:
                return Options{9, 9, 10};
            case Mode::INTERMEDIATE:
                return Options{16, 16, 40};
            default:
                return Options{16, 30, 99};
        }
    }
};

class Layout {
private:
    static constexpr int ROWS[]{9, 16, 16};
    static constexpr int COLUMNS[]{9, 16, 30};

    static constexpr int WINDOW_WIDTH[]{210, 350, 630};
    static constexpr int WINDOW_HEIGHT[]{276, 416, 416};
    static constexpr int WINDOW_LEFT = 100;
    static constexpr int WINDOW_TOP = 100;

    static constexpr int TILE_SIDE = 20;

    static constexpr int DIGIT_PANEL_WIDTH = 65;
    static constexpr int DIGIT_PANEL_HEIGHT = 37;
    static constexpr int DIGIT_WIDTH = 19;
    static constexpr int DIGIT_HEIGHT = 33;
    static constexpr int DIGIT_PANEL_HORZ_MARGIN = (DIGIT_PANEL_WIDTH - (3 * DIGIT_WIDTH)) / 4;
    static constexpr int DIGIT_PANEL_VERT_MARGIN = (DIGIT_PANEL_HEIGHT - DIGIT_HEIGHT) / 2;

    static constexpr int DIGIT_PANEL_OFFSET[]{16, 20, 20};

    static constexpr int TIMER_TOP = 21;

    static constexpr int FLAGS_TOP = 21;

    static constexpr int FACE_WIDTH = 42;
    static constexpr int FACE_HEIGHT = 42;
    static constexpr int FACE_TOP = 19;

    static constexpr int GRID_LEFT = 15;
    static constexpr int GRID_TOP = 81;

    const Mode::Enum mode;
public:
    explicit Layout(const Mode::Enum mode) : mode(mode) {

    }

    static int getTileSide() {
        return TILE_SIDE;
    }

    [[nodiscard]] SDL_Rect getDigitPanel(int left, int top) const {
        return {left, top, DIGIT_PANEL_WIDTH, DIGIT_PANEL_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getFlagsDigitPanel() const {
        return getDigitPanel(DIGIT_PANEL_OFFSET[mode], FLAGS_TOP);
    }

    [[nodiscard]] SDL_Rect getTimerDigitPanel() const {
        return getDigitPanel(WINDOW_WIDTH[mode] - DIGIT_PANEL_OFFSET[mode] - DIGIT_PANEL_WIDTH, TIMER_TOP);
    }

    [[nodiscard]] SDL_Rect getDigit(int left, int top, int position) const {
        return {
                left + DIGIT_PANEL_HORZ_MARGIN * (position + 1) + DIGIT_WIDTH * position,
                top + DIGIT_PANEL_VERT_MARGIN,
                DIGIT_WIDTH,
                DIGIT_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getFlagsDigit(int position) const {
        return getDigit(DIGIT_PANEL_OFFSET[mode], FLAGS_TOP, position);
    }

    [[nodiscard]] SDL_Rect getTimerDigit(int position) const {
        return getDigit(WINDOW_WIDTH[mode] - DIGIT_PANEL_OFFSET[mode] - DIGIT_PANEL_WIDTH, TIMER_TOP, position);
    }

    [[nodiscard]] SDL_Rect getFace() const {
        return {WINDOW_WIDTH[mode] / 2 - FACE_WIDTH / 2, FACE_TOP, FACE_WIDTH, FACE_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getTile(int gridX, int gridY, int row, int col) const {
        return {gridX + col * TILE_SIDE, gridY + row * TILE_SIDE, TILE_SIDE, TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect getGrid() const {
        return {
                GRID_LEFT,
                GRID_TOP,
                COLUMNS[mode] * TILE_SIDE,
                ROWS[mode] * TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect getBackground() const {
        return {0, 0, WINDOW_WIDTH[mode], WINDOW_HEIGHT[mode]};
    }

    [[nodiscard]] SDL_Rect getWindow() const {
        return {WINDOW_LEFT, WINDOW_TOP, WINDOW_WIDTH[mode], WINDOW_HEIGHT[mode]};
    }
};

class ClockTimer {
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;
    std::chrono::time_point<clock_t> start;
public:
    ClockTimer() : start(clock_t::now()) {
    }

    void reset() {
        start = clock_t::now();
    }

    [[nodiscard]] double elapsed() const {
        return std::chrono::duration_cast<second_t>(clock_t::now() - start).count();
    }
};

class Random {
private:
    std::mt19937 mersenne;
public:
    Random() : mersenne{static_cast<std::mt19937::result_type>(std::time(nullptr))} {

    }

    int randomInt(int min, int max) {
        std::uniform_int_distribution die{min, max};
        return die(mersenne);
    }
};

class Texture {
private:
    SDL_Renderer *ren;
    SDL_Texture *texture;
public:
    Texture() : Texture(nullptr, nullptr) {

    }

    Texture(SDL_Renderer *ren, SDL_Texture *texture) : ren(ren), texture(texture) {

    }

    ~Texture() {
        SDL_DestroyTexture(texture);
    }

    void render(SDL_Rect *rect) {
        SDL_RenderCopy(ren, texture, nullptr, rect);
    }
};

using TexturePtr = std::shared_ptr<Texture>;

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

namespace minesweeper {
class Context
{
public:
    sdl::IWindowPtr window;
    sdl::IRendererPtr renderer;
    sdl::IImageRepoPtr imageRepo;
    Layout layout;
    Mode::Enum mode;
};
using ContextPtr = std::shared_ptr<Context>;

class Timer : public DigitPanel, public GameStateListener
{
public:
    Timer(const ContextPtr &context) : DigitPanel(context, context->layout.getTimerDigitPanel()) {}

    SDL_Rect getDigitRect(int position) override
    {
        return _context->layout.getTimerDigit(position);
    }

    int getDisplayValue() override { return running ? static_cast<int>(timer.elapsed()) : elapsed; }

    void onStateChange(GameState state) override
    {
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

private:
    const ContextPtr _context;
    ClockTimer timer;
    bool running = false;
    int elapsed = 0;
};

using TimerPtr = std::shared_ptr<Timer>;

class Sprite
{
public:
    Sprite(const ContextPtr &context, const SDL_Rect &boundingBox)
        : _context(context), _boundingBox(boundingBox)
    {}

    virtual ~Sprite() = default;

public:
    void onClick(const SDL_MouseButtonEvent &evt)
    {
        if (contains(evt.x, evt.y)) {
            handleClick(evt);
        }
    }

    virtual void render() {}

    virtual void handleClick(const SDL_MouseButtonEvent &evt) {}

protected:
    bool contains(int x, int y) const
    {
        auto pt{SDL_Point{x, y}};
        return SDL_PointInRect(&pt, &_boundingBox);
    }

protected:
    const ContextPtr _context;
    const SDL_Rect _boundingBox;
};
using SpritePtr = std::shared_ptr<Sprite>;

class DigitPanel : public Sprite
{
public:
    DigitPanel(const ContextPtr &context, const SDL_Rect &boundingBox)
        : Sprite(context, boundingBox)
    {}

    virtual SDL_Rect getDigitRect(int position) = 0;
    virtual int getDisplayValue() = 0;

    void render() override
    {
        getTexture()->render(_boundingBox);

        auto value = getDisplayValue();
        auto onesDigit = value % 10;
        auto tensDigit = (value / 10) % 10;
        auto hundredsDigit = (value / 100) % 10;

        SDL_Rect rect = getDigitRect(0);
        getDigit(hundredsDigit)->render(rect);

        rect = getDigitRect(1);
        getDigit(tensDigit)->render(rect);

        rect = getDigitRect(2);
        getDigit(onesDigit)->render(rect);
    }

private:
    sdl::ITexturePtr getTexture()
    {
        return _context->imageRepo->get("minesweeper_digit_panel");
    }
    sdl::ITexturePtr getDigit(unsigned digit)
    {
        switch (digit) {
        case 1:
            return _context->imageRepo->get("minesweeper_digit_one");
        case 2:
            return _context->imageRepo->get("minesweeper_digit_two");
        case 3:
            return _context->imageRepo->get("minesweeper_digit_three");
        case 4:
            return _context->imageRepo->get("minesweeper_digit_four");
        case 5:
            return _context->imageRepo->get("minesweeper_digit_five");
        case 6:
            return _context->imageRepo->get("minesweeper_digit_six");
        case 7:
            return _context->imageRepo->get("minesweeper_digit_seven");
        case 8:
            return _context->imageRepo->get("minesweeper_digit_eight");
        case 9:
            return _context->imageRepo->get("minesweeper_digit_nine");
        default:
            return _context->imageRepo->get("minesweeper_digit_zero");
        }
    }

};

class Background : public Sprite
{
public:
    Background(const ContextPtr &context) : Sprite(context, context->layout.getBackground()) {}
    void render() override { getTexture()->render(_boundingBox); }

private:
    sdl::ITexturePtr getTexture() const
    {
        switch (_context->mode) {
        case Mode::BEGINNER:
            return _context->imageRepo->get("minesweeper_bg_beginner");
        case Mode::INTERMEDIATE:
            return _context->imageRepo->get("minesweeper_bg_intermediate");
        default:
            return _context->imageRepo->get("minesweeper_bg_expert");
        }
    }
};

class Game
{
public:
    using Sprites = std::vector<SpritePtr>;

public:
    Game(const ContextPtr &context) : _context(context)
    {
        auto background = std::make_shared<Background>(_context);
        _sprites.push_back(background);
        auto timer{std::make_shared<Timer>(context)};

    }
    void run(const Options &options) {}

private:
    const ContextPtr _context;
    Sprites _sprites;
};
} // namespace minesweeper

int main(int argc, char **argv) {
    Mode::Enum mode = Mode::parse(argc > 1 ? *argv[1] : 'e');
    Options options{Options::getOptions(mode)};
    Layout layout{mode};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
#define USE_INTERFACES
#ifdef USE_INTERFACES
    auto window = sdl::IWindow::create("Minesweeper", layout.getWindow(), SDL_WINDOW_SHOWN);
    auto renderer = window->createRenderer();
    auto imageRepo = renderer->createImageRepo("images/");
    auto context = minesweeper::Context{window, renderer, imageRepo, layout, mode};

    auto contextPtr = std::make_shared<minesweeper::Context>(context);
    auto game{minesweeper::Game(contextPtr)};
    game.run(options);

#else
    Window window{layout, mode};
    Renderer renderer{window.createRenderer()};
    ImageRepo imageRepo{renderer.createImageRepo("images/")};

    Game game{imageRepo, renderer, options, layout};
    game.run();
#endif
    SDL_Quit();
}
