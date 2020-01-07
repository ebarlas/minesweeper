#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <random>
#include <set>
#include "SDL.h"

constexpr int WINDOW_WIDTH = 480;
constexpr int WINDOW_HEIGHT = 336;
constexpr int WINDOW_LEFT = 100;
constexpr int WINDOW_TOP = 100;

constexpr int TILE_SIDE = 15;

class Options {
private:
    const int rows;
    const int columns;
    const int mines;
public:
    Options(int rows, int columns, int mines) : rows(rows), columns(columns), mines(mines) {

    }

    int getTiles() const {
        return rows * columns;
    }

    int getMines() const {
        return mines;
    }

    int getBlanks() const {
        return getTiles() - mines;
    }

    int getRows() const {
        return rows;
    }

    int getColumns() const {
        return columns;
    }

    bool isValid(int row, int col) const {
        return row >= 0 && row < rows && col >= 0 && col < columns;
    }

    void forEachNeighbor(int row, int col, const std::function<void(int, int)> &fn) const {
        for (int r = row - 1; r <= row + 1; r++) {
            for (int c = col - 1; c <= col + 1; c++) {
                if (r != row || c != col) {
                    if (isValid(r, c)) {
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

    double elapsed() const {
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

class ImageRepo {
private:
    SDL_Renderer *ren;
    const char *dir;

    std::map<std::string, SDL_Texture *> images;

    SDL_Texture *load(const char *name) {
        std::string file{dir};
        file.append(name);
        file.append(".bmp");
        SDL_Surface *image = SDL_LoadBMP(file.c_str());
        SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);
        SDL_FreeSurface(image);
        return texture;
    }

    SDL_Texture *get(const char *name) {
        SDL_Texture *image = images[name];
        return image ? image : images[name] = load(name);
    }

public:
    ImageRepo(SDL_Renderer *ren, const char *dir) : ren(ren), dir(dir) {

    }

    ~ImageRepo() {
        for (auto &entry : images)
            SDL_DestroyTexture(entry.second);
    }

    SDL_Texture *getBackground() {
        return get("minesweeper_bg");
    }

    SDL_Texture *getDigitPanel() {
        return get("minesweeper_digit_panel");
    }

    SDL_Texture *getDigit(int digit) {
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

    SDL_Texture *getFacePlaying() {
        return get("minesweeper_face_playing");
    }

    SDL_Texture *getFaceWin() {
        return get("minesweeper_face_win");
    }

    SDL_Texture *getFaceLose() {
        return get("minesweeper_face_lose");
    }

    SDL_Texture *getTileFlag() {
        return get("minesweeper_tile_flag");
    }

    SDL_Texture *getTileMine() {
        return get("minesweeper_tile_mine");
    }

    SDL_Texture *getTile() {
        return get("minesweeper_tile");
    }

    SDL_Texture *getTile(int num) {
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
    Renderer(SDL_Window *win) : ren(SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) {

    }

    ~Renderer() {
        SDL_DestroyRenderer(ren);
    }

    ImageRepo createImageRepo(const char *dir) {
        return ImageRepo{ren, dir};
    }

    void render(SDL_Texture *texture, SDL_Rect *rect = nullptr) {
        SDL_RenderCopy(ren, texture, nullptr, rect);
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
               && x <= boundingBox.x + boundingBox.w
               && y >= boundingBox.y
               && y <= boundingBox.y + boundingBox.h;
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
};

class DigitPanel : public Sprite {
protected:
    static constexpr int DIGIT_PANEL_WIDTH = 65;
    static constexpr int DIGIT_PANEL_HEIGHT = 37;
    static constexpr int DIGIT_WIDTH = 19;
    static constexpr int DIGIT_HEIGHT = 33;
    static constexpr int DIGIT_PANEL_HORZ_MARGIN = (DIGIT_PANEL_WIDTH - (3 * DIGIT_WIDTH)) / 4;
    static constexpr int DIGIT_PANEL_VERT_MARGIN = (DIGIT_PANEL_HEIGHT - DIGIT_HEIGHT) / 2;

    DigitPanel(ImageRepo &imageRepo, Renderer &renderer, int left, int top)
            : Sprite(imageRepo, renderer, {left, top, DIGIT_PANEL_WIDTH, DIGIT_PANEL_HEIGHT}) {

    }

    SDL_Rect getDigitRect(int position) {
        return {
                boundingBox.x + DIGIT_PANEL_HORZ_MARGIN * (position + 1) + DIGIT_WIDTH * position,
                boundingBox.y + DIGIT_PANEL_VERT_MARGIN,
                DIGIT_WIDTH,
                DIGIT_HEIGHT};
    }

public:
    void render() override {
        renderer.render(imageRepo.getDigitPanel(), &boundingBox);

        int value = getDisplayValue();
        int onesDigit = value % 10;
        int tensDigit = (value / 10) % 10;
        int hundredsDigit = (value / 100) % 10;

        SDL_Rect rect = getDigitRect(0);
        renderer.render(imageRepo.getDigit(hundredsDigit), &rect);

        rect = getDigitRect(1);
        renderer.render(imageRepo.getDigit(tensDigit), &rect);

        rect = getDigitRect(2);
        renderer.render(imageRepo.getDigit(onesDigit), &rect);
    }

    virtual int getDisplayValue() = 0;
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
};

class Timer : public DigitPanel, public GameStateListener {
private:
    static constexpr int TIMER_LEFT = WINDOW_WIDTH - 25 - DIGIT_PANEL_WIDTH;
    static constexpr int TIMER_TOP = 21;

    ClockTimer timer;
    bool running;
    int elapsed;
public:
    Timer(ImageRepo &imageRepo, Renderer &renderer)
            : DigitPanel(imageRepo, renderer, TIMER_LEFT, TIMER_TOP), running(false), elapsed(0) {

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

class TileListener {
public:
    virtual void onReveal(bool mine, bool adjacentMines) {

    }

    virtual void onClear() {

    }

    virtual void onFlag(bool flagged) {

    }
};

class FlagStateListener {
public:
    virtual void onFlagStateChange(bool exhausted) = 0;
};

class FlagCounter : public DigitPanel, public GameStateListener, public TileListener {
private:
    static constexpr int FLAGS_LEFT = 25;
    static constexpr int FLAGS_TOP = 21;

    int flags;
    const Options &options;

    std::vector<FlagStateListener *> listeners;

    void notifyListeners(bool exhausted) {
        for (auto listener : listeners)
            listener->onFlagStateChange(exhausted);
    }

public:
    FlagCounter(ImageRepo &imageRepo, Renderer &renderer, const Options &options)
            : DigitPanel(imageRepo, renderer, FLAGS_LEFT, FLAGS_TOP), flags(options.getMines()), options(options) {

    }

    void setListeners(const std::vector<FlagStateListener *> &v) {
        listeners = v;
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

class Button : public Sprite, public TileListener {
private:
    static constexpr int FACE_WIDTH = 42;
    static constexpr int FACE_HEIGHT = 42;
    static constexpr int FACE_LEFT = WINDOW_WIDTH / 2 - FACE_WIDTH / 2;
    static constexpr int FACE_TOP = 19;

    GameState state;
    int revealed;
    const Options &options;

    std::vector<GameStateListener *> listeners;

    SDL_Texture *getFaceImage() {
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
        for (auto listener : listeners)
            listener->onStateChange(state);
    }

public:
    Button(ImageRepo &imageRepo, Renderer &renderer, const Options &options) :
            Sprite(imageRepo, renderer, {FACE_LEFT, FACE_TOP, FACE_WIDTH, FACE_HEIGHT}),
            state(GameState::INIT),
            revealed(0),
            options(options) {

    }

    void setListeners(const std::vector<GameStateListener *> &v) {
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
        renderer.render(getFaceImage(), &boundingBox);
    }
};

class MineField {
private:
    Random random;
    std::set<int> mines;
    const Options &options;
public:
    MineField(const Options &options) : options(options) {
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

    std::vector<TileListener *> listeners;

    void tryReveal() {
        if (gameOver || flagged || revealed)
            return;
        revealed = true;
        for (auto listener : listeners)
            listener->onReveal(mine, adjacentMines > 0);
    }

    void tryToggleFlag() {
        if (gameOver || revealed)
            return;
        if (!flagged && !flagRemaining)
            return;
        flagged = !flagged;
        for (auto listener : listeners)
            listener->onFlag(flagged);
    }

    void tryClear() {
        if (adjacentFlags == adjacentMines)
            for (auto listener : listeners)
                listener->onClear();
    }

public:
    Tile(ImageRepo &imageRepo, Renderer &renderer, const SDL_Rect &gridRect, int row, int col, int adjacentMines,
         bool mine) :
            Sprite(imageRepo, renderer, {
                    gridRect.x + col * TILE_SIDE,
                    gridRect.y + row * TILE_SIDE,
                    TILE_SIDE,
                    TILE_SIDE}),
            adjacentMines(adjacentMines),
            adjacentFlags(0),
            mine(mine),
            flagged(false),
            revealed(false),
            gameOver(false),
            flagRemaining(true) {

    }

    void setListeners(const std::vector<TileListener *> &v) {
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
                renderer.render(imageRepo.getTileMine(), &boundingBox);
            } else {
                renderer.render(imageRepo.getTile(adjacentMines), &boundingBox);
            }
        } else if (flagged) {
            renderer.render(imageRepo.getTileFlag(), &boundingBox);
        } else {
            renderer.render(imageRepo.getTile(), &boundingBox);
        }
    }
};

class Grid : public Sprite, public GameStateListener, public FlagStateListener {
private:
    static constexpr int GRID_LEFT = 15;
    static constexpr int GRID_TOP = 81;
    static constexpr int GRID_WIDTH = 450;
    static constexpr int GRID_HEIGHT = 240;

    std::vector<std::vector<Tile>> grid;
    MineField mineField;
    const Options &options;

    void forEachTile(const std::function<void(Tile &)> &fn) {
        for (auto &row : grid)
            for (auto &col : row)
                fn(col);
    }

public:
    Grid(ImageRepo &imageRepo, Renderer &renderer, const Options &options) :
            Sprite(imageRepo, renderer, {
                    GRID_LEFT + (GRID_WIDTH - options.getColumns() * TILE_SIDE) / 2,
                    GRID_TOP + (GRID_HEIGHT - options.getRows() * TILE_SIDE) / 2,
                    options.getColumns() * TILE_SIDE,
                    options.getRows() * TILE_SIDE}),
            mineField(options),
            options(options) {
        grid.reserve(options.getRows());
        for (int r = 0; r < options.getRows(); r++) {
            std::vector<Tile> v;
            v.reserve(options.getColumns());
            for (int c = 0; c < options.getColumns(); c++) {
                int adjMin = mineField.adjacentMines(r, c);
                bool mine = mineField.mineAt(r, c);
                v.emplace_back(Tile{imageRepo, renderer, boundingBox, r, c, adjMin, mine});
            }
            grid.push_back(v);
        }
    }

    void setListeners(const std::vector<TileListener *> &v) {
        for (int r = 0; r < options.getRows(); r++) {
            for (int c = 0; c < options.getColumns(); c++) {
                std::vector<TileListener *> listeners = v;
                auto fn = [&listeners, this](int r, int c) { listeners.push_back(&grid[r][c]); };
                options.forEachNeighbor(r, c, fn);
                grid[r][c].setListeners(listeners);
            }
        }
    }

    void handleClick(SDL_MouseButtonEvent evt) override {
        int col = (evt.x - boundingBox.x) / TILE_SIDE;
        int row = (evt.y - boundingBox.y) / TILE_SIDE;
        if (options.isValid(row, col))
            grid[row][col].handleClick(evt);
    }

    void onFlagStateChange(bool exhausted) override {
        forEachTile([exhausted](Tile &t) { t.onFlagStateChange(exhausted); });
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT) {
            mineField.reset();
            for (int r = 0; r < options.getRows(); r++) {
                for (int c = 0; c < options.getColumns(); c++) {
                    grid[r][c].reset(mineField.adjacentMines(r, c), mineField.mineAt(r, c));
                }
            }
        }

        forEachTile([state](Tile &t) { t.onStateChange(state); });
    }

    void render() override {
        forEachTile([](Tile &t) { t.render(); });
    }
};

class Background : public Sprite {
public:
    Background(ImageRepo &imageRepo, Renderer &renderer) : Sprite(imageRepo, renderer,
                                                                  {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}) {

    }

    void render() override {
        renderer.render(imageRepo.getBackground());
    }
};

class Game {
private:
    Background background;
    Timer timer;
    FlagCounter flagCounter;
    Button button;
    Grid grid;

    Renderer &renderer;

    std::vector<Sprite *> sprites;

    void onClick(SDL_MouseButtonEvent evt) {
        for (auto sprite : sprites)
            sprite->onClick(evt);
    }

    void render() {
        for (auto sprite : sprites)
            sprite->render();
        renderer.repaint();
    }

public:
    Game(ImageRepo &imageRepo, Renderer &renderer, const Options &options) :
            background{imageRepo, renderer},
            timer{imageRepo, renderer},
            flagCounter{imageRepo, renderer, options},
            button{imageRepo, renderer, options},
            grid{imageRepo, renderer, options},
            renderer(renderer) {
        std::vector<GameStateListener *> gameStateListeners{&grid, &timer, &flagCounter};
        button.setListeners(gameStateListeners);

        std::vector<TileListener *> tileRevealListeners{&button, &flagCounter};
        grid.setListeners(tileRevealListeners);

        std::vector<FlagStateListener *> flagStateListeners{&grid};
        flagCounter.setListeners(flagStateListeners);

        sprites.push_back(&background);
        sprites.push_back(&timer);
        sprites.push_back(&flagCounter);
        sprites.push_back(&button);
        sprites.push_back(&grid);
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
            WINDOW_LEFT,
            WINDOW_TOP,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
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

    SDL_Init(SDL_INIT_VIDEO);
    Window window;
    Renderer renderer{window.createRenderer()};
    ImageRepo imageRepo{renderer.createImageRepo("images/")};

    Game game{imageRepo, renderer, options};
    game.run();

    SDL_Quit();
}