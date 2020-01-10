#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <random>
#include <set>
#include <functional>
#include <memory>
#include "SDL.h"

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

    static Options getOptions(char c) {
        switch (c) {
            case 'b':
                return Options{9, 9, 10};
            case 'i':
                return Options{16, 16, 40};
            default:
                return Options{16, 30, 99};
        }
    }
};

class Layout {
public:
    static constexpr int WINDOW_WIDTH = 480;
    static constexpr int WINDOW_HEIGHT = 336;
    static constexpr int WINDOW_LEFT = 100;
    static constexpr int WINDOW_TOP = 100;

    static constexpr int TILE_SIDE = 15;

    static constexpr int DIGIT_PANEL_WIDTH = 65;
    static constexpr int DIGIT_PANEL_HEIGHT = 37;
    static constexpr int DIGIT_WIDTH = 19;
    static constexpr int DIGIT_HEIGHT = 33;
    static constexpr int DIGIT_PANEL_HORZ_MARGIN = (DIGIT_PANEL_WIDTH - (3 * DIGIT_WIDTH)) / 4;
    static constexpr int DIGIT_PANEL_VERT_MARGIN = (DIGIT_PANEL_HEIGHT - DIGIT_HEIGHT) / 2;

    static constexpr int TIMER_LEFT = WINDOW_WIDTH - 25 - DIGIT_PANEL_WIDTH;
    static constexpr int TIMER_TOP = 21;

    static constexpr int FLAGS_LEFT = 25;
    static constexpr int FLAGS_TOP = 21;

    static constexpr int FACE_WIDTH = 42;
    static constexpr int FACE_HEIGHT = 42;
    static constexpr int FACE_LEFT = WINDOW_WIDTH / 2 - FACE_WIDTH / 2;
    static constexpr int FACE_TOP = 19;

    static constexpr int GRID_LEFT = 15;
    static constexpr int GRID_TOP = 81;
    static constexpr int GRID_WIDTH = 450;
    static constexpr int GRID_HEIGHT = 240;

    const int rows;
    const int columns;

    Layout(const int rows, const int columns) : rows(rows), columns(columns) {

    }

    [[nodiscard]] SDL_Rect getDigitPanel(int left, int top) const {
        return {left, top, DIGIT_PANEL_WIDTH, DIGIT_PANEL_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getFlagsDigitPanel() const {
        return getDigitPanel(FLAGS_LEFT, FLAGS_TOP);
    }

    [[nodiscard]] SDL_Rect getTimerDigitPanel() const {
        return getDigitPanel(TIMER_LEFT, TIMER_TOP);
    }

    [[nodiscard]] SDL_Rect getDigit(int left, int top, int position) const {
        return {
                left + DIGIT_PANEL_HORZ_MARGIN * (position + 1) + DIGIT_WIDTH * position,
                top + DIGIT_PANEL_VERT_MARGIN,
                DIGIT_WIDTH,
                DIGIT_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getFlagsDigit(int position) const {
        return getDigit(FLAGS_LEFT, FLAGS_TOP, position);
    }

    [[nodiscard]] SDL_Rect getTimerDigit(int position) const {
        return getDigit(TIMER_LEFT, TIMER_TOP, position);
    }

    [[nodiscard]] SDL_Rect getFace() const {
        return {FACE_LEFT, FACE_TOP, FACE_WIDTH, FACE_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getTile(int gridX, int gridY, int row, int col) const {
        return {gridX + col * TILE_SIDE, gridY + row * TILE_SIDE, TILE_SIDE, TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect getGrid() const {
        return {
                GRID_LEFT + (GRID_WIDTH - columns * TILE_SIDE) / 2,
                GRID_TOP + (GRID_HEIGHT - rows * TILE_SIDE) / 2,
                columns * TILE_SIDE,
                rows * TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect getBackground() const {
        return {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
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
    ImageRepo(SDL_Renderer *ren, const char *dir) : ren(ren), dir(dir) {

    }

    TexturePtr getBackground() {
        return get("minesweeper_bg");
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
public:
    explicit Renderer(SDL_Window *win)
            : ren(SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) {
        if(ren == nullptr) {
            ren = SDL_CreateRenderer(win, -1, 0);
        }
    }

    ~Renderer() {
        SDL_DestroyRenderer(ren);
    }

    ImageRepo createImageRepo(const char *dir) {
        return ImageRepo{ren, dir};
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
        int col = (evt.x - boundingBox.x) / Layout::TILE_SIDE;
        int row = (evt.y - boundingBox.y) / Layout::TILE_SIDE;
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
public:
    Window() : win(SDL_CreateWindow(
            "Minesweeper",
            Layout::WINDOW_LEFT,
            Layout::WINDOW_TOP,
            Layout::WINDOW_WIDTH,
            Layout::WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN)) {

    }

    ~Window() {
        SDL_DestroyWindow(win);
    }

    Renderer createRenderer() {
        return Renderer{win};
    }
};

int main(int argc, char **argv) {
    char mode = argc > 1 ? *argv[1] : 'e';
    Options options{Options::getOptions(mode)};
    Layout layout{options.getRows(), options.getColumns()};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    Window window;
    Renderer renderer{window.createRenderer()};
    ImageRepo imageRepo{renderer.createImageRepo("images/")};

    Game game{imageRepo, renderer, options, layout};
    game.run();

    SDL_Quit();
}
