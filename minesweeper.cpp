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

constexpr int COLUMNS = 30;
constexpr int ROWS = 16;

constexpr int TILES_LEFT = 15;
constexpr int TILES_TOP = 81;
constexpr int TILE_SIDE = 15;

constexpr int DIGIT_PANEL_WIDTH = 65;
constexpr int DIGIT_PANEL_HEIGHT = 37;

constexpr int DIGIT_WIDTH = 19;
constexpr int DIGIT_HEIGHT = 33;
constexpr int DIGIT_PANEL_HORZ_MARGIN = (DIGIT_PANEL_WIDTH - (3 * DIGIT_WIDTH)) / 4;
constexpr int DIGIT_PANEL_VERT_MARGIN = (DIGIT_PANEL_HEIGHT - DIGIT_HEIGHT) / 2;

constexpr int FLAGS_LEFT = 25;
constexpr int FLAGS_TOP = 21;

constexpr int TIMER_LEFT = WINDOW_WIDTH - 25 - DIGIT_PANEL_WIDTH;
constexpr int TIMER_TOP = 21;

constexpr int FACE_WIDTH = 42;
constexpr int FACE_HEIGHT = 42;
constexpr int FACE_LEFT = WINDOW_WIDTH / 2 - FACE_WIDTH / 2;
constexpr int FACE_TOP = 19;

constexpr int NUM_MINES = 99;

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

    void close() {
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
    Renderer(SDL_Renderer *ren) : ren(ren) {

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
    int flags;

    std::vector<FlagStateListener *> listeners;

    void notifyListeners(bool exhausted) {
        for (auto listener : listeners)
            listener->onFlagStateChange(exhausted);
    }

public:
    FlagCounter(ImageRepo &imageRepo, Renderer &renderer)
            : DigitPanel(imageRepo, renderer, FLAGS_LEFT, FLAGS_TOP), flags(NUM_MINES) {

    }

    void setListeners(const std::vector<FlagStateListener *> &v) {
        listeners = v;
    }

    int getDisplayValue() override {
        return flags;
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT)
            flags = NUM_MINES;
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
    GameState state;
    int revealed;

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
    Button(ImageRepo &imageRepo, Renderer &renderer) :
            Sprite(imageRepo, renderer, {FACE_LEFT, FACE_TOP, FACE_WIDTH, FACE_HEIGHT}),
            state(GameState::INIT),
            revealed(0) {

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
            if (revealed == ROWS * COLUMNS - NUM_MINES) {
                state = GameState::WON;
                notifyListeners();
            }
        }
    }

    void render() override {
        renderer.render(getFaceImage(), &boundingBox);
    }
};

class RowCol {
private:
    const int row;
    const int col;

    static void addNeighbor(int row, int col, std::vector<RowCol> &v) {
        if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS)
            v.push_back(RowCol{row, col});
    }

public:
    RowCol(int row, int col) : row(row), col(col) {

    }

    int getRow() const {
        return row;
    }

    int getCol() const {
        return col;
    }

    static std::vector<RowCol> neighbors(int row, int col) {
        std::vector<RowCol> v;
        addNeighbor(row - 1, col - 1, v);
        addNeighbor(row - 1, col, v);
        addNeighbor(row - 1, col + 1, v);
        addNeighbor(row, col - 1, v);
        addNeighbor(row, col + 1, v);
        addNeighbor(row + 1, col - 1, v);
        addNeighbor(row + 1, col, v);
        addNeighbor(row + 1, col + 1, v);
        return v;
    }
};

class MineSet {
private:
    Random random;
    std::set<int> mines;
public:
    MineSet() {
        reset();
    }

    void reset() {
        mines.clear();
        for (int i = 1; i <= NUM_MINES; i++) {
            while (mines.size() < i) {
                mines.insert(random.randomInt(0, COLUMNS * ROWS - 1));
            }
        }
    }

    bool mineAt(int row, int col) {
        int projection = row * COLUMNS + col;
        return mines.find(projection) != mines.end();
    }

    int adjacentMines(int row, int col) {
        int sum = 0;
        for (auto rc : RowCol::neighbors(row, col))
            if (mineAt(rc.getRow(), rc.getCol()))
                sum++;
        return sum;
    }
};

class Tile : public Sprite, public TileListener, public GameStateListener, public FlagStateListener {
private:
    int row;
    int col;
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
    Tile(ImageRepo &imageRepo, Renderer &renderer, int row, int col, int adjacentMines, bool mine) :
            Sprite(imageRepo, renderer,
                   {TILES_LEFT + col * TILE_SIDE, TILES_TOP + row * TILE_SIDE, TILE_SIDE, TILE_SIDE}),
            row(row),
            col(col),
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
    std::vector<std::vector<Tile>> grid;
    MineSet mineSet;

public:
    Grid(ImageRepo &imageRepo, Renderer &renderer)
            : Sprite(imageRepo, renderer,
                     {TILES_LEFT, TILES_TOP, TILES_LEFT + COLUMNS * TILE_SIDE, TILES_TOP + ROWS * TILE_SIDE}) {
        for (int r = 0; r < ROWS; r++) {
            std::vector<Tile> v;
            for (int c = 0; c < COLUMNS; c++) {
                v.push_back(Tile{imageRepo, renderer, r, c, mineSet.adjacentMines(r, c), mineSet.mineAt(r, c)});
            }
            grid.push_back(v);
        }
    }

    void setListeners(const std::vector<TileListener *> &v) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLUMNS; c++) {
                std::vector<TileListener *> listeners = v;
                for (auto rc : RowCol::neighbors(r, c))
                    listeners.push_back(&grid[rc.getRow()][rc.getCol()]);
                grid[r][c].setListeners(listeners);
            }
        }
    }

    void handleClick(SDL_MouseButtonEvent evt) override {
        int col = (evt.x - TILES_LEFT) / TILE_SIDE;
        int row = (evt.y - TILES_TOP) / TILE_SIDE;
        if (col < COLUMNS && row < ROWS)
            grid[row][col].handleClick(evt);
    }

    void onFlagStateChange(bool exhausted) override {
        for (auto &row : grid)
            for (auto &col : row)
                col.onFlagStateChange(exhausted);
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT) {
            mineSet.reset();
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLUMNS; c++) {
                    grid[r][c].reset(mineSet.adjacentMines(r, c), mineSet.mineAt(r, c));
                }
            }
        }

        for (auto &row : grid) {
            for (auto &col : row) {
                col.onStateChange(state);
            }
        }
    }

    void render() override {
        for (auto &row : grid) {
            for (auto &col : row) {
                col.render();
            }
        }
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
public:
    Game(ImageRepo &imageRepo, Renderer &renderer) :
            background{imageRepo, renderer},
            timer{imageRepo, renderer},
            flagCounter{imageRepo, renderer},
            button{imageRepo, renderer},
            grid{imageRepo, renderer},
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

    void onClick(SDL_MouseButtonEvent evt) {
        for (auto sprite : sprites)
            sprite->onClick(evt);
    }

    void render() {
        for (auto sprite : sprites)
            sprite->render();
        renderer.repaint();
    }
};

SDL_Window *createWindow() {
    return SDL_CreateWindow(
            "Minesweeper",
            WINDOW_LEFT,
            WINDOW_TOP,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN);
}

SDL_Renderer *createRenderer(SDL_Window *win) {
    return SDL_CreateRenderer(
            win,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void handleEvents(Game &game) {
    while (true) {
        SDL_Event e;
        int res = SDL_WaitEventTimeout(&e, 100);
        if (res != 0) {
            if (e.type == SDL_QUIT) {
                break;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                game.onClick(e.button);
                game.render();
            }
        } else {
            // render on timeout
            game.render();
        }
    }
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = createWindow();
    SDL_Renderer *ren = createRenderer(win);

    ImageRepo imageRepo{ren, "images/"};
    Renderer renderer{ren};
    Game game{imageRepo, renderer};
    game.render();

    handleEvents(game);

    imageRepo.close();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}