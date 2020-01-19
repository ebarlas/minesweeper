
#include "GameContext.h"
#include "Mode.h"
#include "DigitalPanel.h"
#include "Options.h"

#include "util/RandomFactory.h"
#include "util/IElapsedTime.h"
#include "util/Coordinates.h"
#include "SDL2/framework/IWindow.h"
#include "SDL2/framework/IRenderer.h"
#include "SDL2/framework/IImageRepo.h"
#include "SDL2/framework/ITexture.h"

#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <memory>
#include "SDL.h"


namespace minesweeper {

enum class GameState {
    INIT,
    PLAYING,
    WON,
    LOST
};

class IGameStateListener {
public:
    virtual ~IGameStateListener() = default;

public:
    virtual void onStateChange(GameState state) = 0;

};
using IGameStateListenerPtr = std::shared_ptr<IGameStateListener>;
using IGameStateListenerWPtr = std::weak_ptr<IGameStateListener>;

class Timer : public DigitPanel, public IGameStateListener
{
public:
    Timer(const ContextPtr &context, const IDigitsPtr& digits) : DigitPanel(context, digits, context->layout.getTimerDigitPanel()) {}

public:
    SDL_Rect getDigitRect(int position) override
    {
        return _context->layout.getTimerDigit(position);
    }

    int getDisplayValue() override {
        return _running ? static_cast<int>(_timer->elapsed()) : _elapsed;
    }

    void onStateChange(GameState state) override
    {
        if (state == GameState::PLAYING) {
            _running = true;
            _timer->reset();
        } else if (state == GameState::WON || state == GameState::LOST) {
            _running = false;
            _elapsed = static_cast<int>(_timer->elapsed());
        } else {
            _running = false;
            _elapsed = 0;
        }
    }

private:
    util::IElapsedTimeUPtr _timer {util::createElapsedTime()};
    bool _running = false;
    unsigned _elapsed = 0;
};

using TimerPtr = std::shared_ptr<Timer>;

class MineField {
public:
    using Coordinate = util::Coordinate;

public:
    MineField(const Options &options) : _options(options) {
        reset();
    }

    void reset() {
        _mines.clear();
        for (unsigned i = 1; i <= _options.getMines(); i++) {
            while (_mines.size() < i) {
                _mines.insert(_random->random(0, _options.getTiles() - 1));
            }
        }
    }

    bool mineAt(int row, int col) {
        int projection = row * _options.getColumns() + col;
        return _mines.find(projection) != _mines.end();
    }

    bool mineAt(const Coordinate& coord) {
        return mineAt(coord.row, coord.column);
    }

    auto adjancentMines( const Coordinate& coord)
    {
        unsigned sum = 0;
        for(const auto& neighbor : _options.neighbors(coord)) {
            sum += mineAt(neighbor) ? 1 : 0;
        }
        return sum;
    }

    int adjacentMines(int row, int col) {
        int sum = 0;
        auto fn = [&sum, this](const util::Coordinate& coord) { sum = sum + (mineAt(coord.row, coord.column) ? 1 : 0); };
        auto neighbors = _options.neighbors(row, col);
        std::for_each(neighbors.begin(), neighbors.end(), fn);
        return sum;
    }

private:
    const Options _options;
    const util::IRandomPtr _random = util::RandomFactory::create();

    std::set<int> _mines;
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

class TileListener {
public:
    virtual void onReveal([[maybe_unused]] bool mine, [[maybe_unused]] bool adjacentMines) {}
    virtual void onClear() {}
    virtual void onFlag([[maybe_unused]] bool flagged) {}
};

using TileListenerPtr = std::shared_ptr<TileListener>;
using TileListenerWPtr = std::weak_ptr<TileListener>;

class IFlagStateListener {
public:
    virtual ~IFlagStateListener() = default;

public:
    virtual void onFlagStateChange(bool exhausted) = 0;
};
using IFlagStateListenerPtr = std::shared_ptr<IFlagStateListener>;
using IFlagStateListenerWPtr = std::weak_ptr<IFlagStateListener>;


class FlagCounter : public DigitPanel, public IGameStateListener, public TileListener
{
public:
    using FlagStateListeners = std::vector<IFlagStateListenerWPtr>;

public:
    FlagCounter(const ContextPtr& context, const IDigitsPtr& digits, const Options options)
        : DigitPanel(context, digits, context->layout.getFlagsDigitPanel())
        , _context(context)
        , _options(options)
        , _flags(options.getMines())
    {}

public:
    void assignListeners(const FlagStateListeners& l) { _listeners = l; }

    SDL_Rect getDigitRect(int position) override {
        return _context->layout.getFlagsDigit(position);
    }

    int getDisplayValue() override {
        return _flags;
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT)
            _flags = _options.getMines();
    }

    void onFlag(bool flagged) override {
        if (flagged) {
            _flags--;
            if (_flags == 0)
                notifyListeners(true);
        } else {
            _flags++;
            if (_flags == 1)
                notifyListeners(false);
        }
    }

private:
    void notifyListeners(bool exhausted) {
        for (auto &listener : _listeners)
            if (auto spt = listener.lock())
                spt->onFlagStateChange(exhausted);
    }

private:
    const ContextPtr _context;
    const Options _options;
    unsigned _flags;
    FlagStateListeners _listeners;
};

class Button : public Sprite, public TileListener
{
public:
    using GameStateListeners = std::vector<IGameStateListenerWPtr>;

public:
    Button(const ContextPtr& context, const Options& options)
        : Sprite(context, context->layout.getFace())
        , _context(context), _options(options)
    {}

    void assignListeners(const GameStateListeners& l) { _listeners = l; }


    void handleClick([[maybe_unused]] const SDL_MouseButtonEvent& evt) override {
        _state = GameState::INIT;
        _revealed = 0;
        notifyListeners();
    }

    void onReveal(bool mine, [[maybe_unused]]bool adjacentMines) override {
        if (mine) {
            _state = GameState::LOST;
            notifyListeners();
        } else {
            if (_state == GameState::INIT) {
                _state = GameState::PLAYING;
                notifyListeners();
            }
            _revealed++;
            if (_revealed == _options.getBlanks()) {
                _state = GameState::WON;
                notifyListeners();
            }
        }
    }

private:
    void notifyListeners() {
        for (auto &listener : _listeners)
            if (auto spt = listener.lock())
                spt->onStateChange(_state);
    }

    sdl::ITexturePtr getFaceImage()
    {
        switch (_state) {
            case GameState::INIT:
            case GameState::PLAYING:
                return _context->imageRepo->get("minesweeper_face_playing");
            case GameState::WON:
                return _context->imageRepo->get("minesweeper_face_win");
            default:
                return _context->imageRepo->get("minesweeper_face_lose");
        }
    }

    void render() override {
        getFaceImage()->render(_boundingBox);
    }

private:
    const ContextPtr _context;
    const Options _options;
    GameState _state;
    unsigned _revealed;
    GameStateListeners _listeners;
};


template<typename T>
class Matrix {
public:
    using Coordinate = util::Coordinate;
    using Coordinates = util::Coordinates;

    using contents_type = std::vector<T>;
    using row_type = Coordinate::ordinate_type;
    using column_type = Coordinate::ordinate_type;

public:
    Matrix(row_type rows, row_type columns, const T &value) : _coordinates(rows, columns), _matrix(rows * columns, value) {}

public:
    T &at(const Coordinate& coord) {
        return _matrix[coord.row * _coordinates.columns() + coord.column];
    }

    T &at(row_type row, row_type col) {
        unsigned n = row * _coordinates.columns() + col;
        return _matrix[n];
    }

    auto rows() const { return _coordinates.rows(); }
    auto columns() const { return _coordinates.columns(); }
    auto begin() { return _matrix.begin(); }
    auto end() { return _matrix.end(); }

    auto neighbors(row_type row, column_type col) const { return _coordinates.neighbors(row, col); }
    auto neighbors(const Coordinate& coord) const { return _coordinates.neighbors(coord); }
    const Coordinates& coordinates() const { return _coordinates; }

private:
    Coordinates _coordinates;
    contents_type _matrix;
};

class Tile : public Sprite, public TileListener, public IGameStateListener, public IFlagStateListener {

public:
    Tile(const ContextPtr& context, const SDL_Rect boundingBox, int adjMines, bool mine) :
            Sprite(context, boundingBox),
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

    void handleClick(const SDL_MouseButtonEvent& evt) override {
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

    sdl::ITexturePtr getTileMine() {
        return _context->imageRepo->get("minesweeper_tile_mine");
    }

    sdl::ITexturePtr  getTile(int num) {
        switch (num) {
            case 1:
                return _context->imageRepo->get("minesweeper_tile_one");
            case 2:
                return _context->imageRepo->get("minesweeper_tile_two");
            case 3:
                return _context->imageRepo->get("minesweeper_tile_three");
            case 4:
                return _context->imageRepo->get("minesweeper_tile_four");
            case 5:
                return _context->imageRepo->get("minesweeper_tile_five");
            case 6:
                return _context->imageRepo->get("minesweeper_tile_six");
            case 7:
                return _context->imageRepo->get("minesweeper_tile_seven");
            case 8:
                return _context->imageRepo->get("minesweeper_tile_eight");
            default:
                return _context->imageRepo->get("minesweeper_tile_none");
        }
    }

    sdl::ITexturePtr getTileFlag() {
        return _context->imageRepo->get("minesweeper_tile_flag");
    }

    sdl::ITexturePtr getTile() {
        return _context->imageRepo->get("minesweeper_tile");
    }

    void render() override {
        if (revealed) {
            if (mine) {
                getTileMine()->render(_boundingBox);
            } else {
                getTile(adjacentMines)->render(_boundingBox);
            }
        } else if (flagged) {
            getTileFlag()->render(_boundingBox);
        } else {
            getTile()->render(_boundingBox);
        }
    }

private:
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


private:
    int adjacentMines;
    int adjacentFlags;
    bool mine;
    bool flagged;
    bool revealed;
    bool gameOver;
    bool flagRemaining;

    std::vector<TileListenerWPtr> listeners;
};

using TilePtr = std::shared_ptr<Tile>;
using TileWPtr = std::weak_ptr<Tile>;


class Grid : public Sprite, public IGameStateListener, public IFlagStateListener {
public:
    //get access to a tile via it's coordinate
    auto& tile(const util::Coordinate& coord) { return tiles.at(coord); }

public:
    Grid(const ContextPtr& context, const Options &options) :
            Sprite(context, context->layout.getGrid()),
            tiles{options.getRows(), options.getColumns(), TilePtr()},
            mineField(options),
            options(options)
    {
        for(const auto& coord : tiles.coordinates()) {
            auto adjacentMines = mineField.adjacentMines(coord.row, coord.column);
            auto mine = mineField.mineAt(coord.row, coord.column);
            auto rect = _context->layout.getTile(_boundingBox.x, _boundingBox.y, coord.row, coord.column);
            tile(coord) = std::make_shared<Tile>(_context, rect, adjacentMines, mine);
        }
    }

    void setListeners(const std::vector<TileListenerWPtr> &v)
    {
        for( const auto& coord : tiles.coordinates()) {
            auto listeners = v;
            for( const auto& neighbor : options.neighbors(coord) ) {
                listeners.push_back(tile(neighbor));
            }
            tile(coord)->setListeners(listeners);
        }
    }

    void handleClick(const SDL_MouseButtonEvent& evt) override {
        int col = (evt.x - _boundingBox.x) / Layout::getTileSide();
        int row = (evt.y - _boundingBox.y) / Layout::getTileSide();
        tiles.at(row, col)->handleClick(evt);
    }

    void onFlagStateChange(bool exhausted) override {
        for(const auto& coord : tiles.coordinates()) {
            tile(coord)->onFlagStateChange(exhausted);
        }
    }

    void onStateChange(GameState state) override {
        if (state == GameState::INIT) {
            mineField.reset();

            for(const auto& coord : tiles.coordinates()) {
                tile(coord)->reset(mineField.adjacentMines(coord.row, coord.column), mineField.mineAt(coord.row, coord.column));
            }
        }
        for(const auto& coord : tiles.coordinates()) {
            tile(coord)->onStateChange(state);
        }
    }

    void render() override {
        for(const auto& coord : tiles.coordinates()) {
            tile(coord)->render();
        }
    }


private:
    Matrix<TilePtr> tiles;
    MineField mineField;
    const Options &options;
};

using GridPtr = std::shared_ptr<Grid>;

class Game
{
public:
    using Sprites = std::vector<SpritePtr>;

public:
    Game(const ContextPtr &context, const Options& options) : _context(context)
    {
        auto digits {std::make_shared<Digits>(_context)};
        auto background {std::make_shared<Background>(_context)};
        auto timer {std::make_shared<Timer>(context, digits)};
        auto flagCounter {std::make_shared<FlagCounter>(_context, digits, options)};
        auto button {std::make_shared<Button>(_context, options)};
        auto grid {std::make_shared<Grid>(_context, options)};

        std::vector<IGameStateListenerWPtr> gameStateListeners{grid, timer, flagCounter};
        button->assignListeners(gameStateListeners);

        std::vector<TileListenerWPtr> tileRevealListeners{button, flagCounter};
        grid->setListeners(tileRevealListeners);

        std::vector<IFlagStateListenerWPtr> flagStateListeners{grid};
        flagCounter->assignListeners(flagStateListeners);

        _sprites.push_back(background);
        _sprites.push_back(timer);
        _sprites.push_back(flagCounter);
        _sprites.push_back(button);
        _sprites.push_back(grid);
    }

    void run()
    {
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

private:
    void onClick(const SDL_MouseButtonEvent& evt) {
        for (auto &sprite : _sprites)
            sprite->onClick(evt);
    }

    void render() {
        for (auto &sprite : _sprites)
            sprite->render();
        _context->renderer->repaint();
    }

private:
    const ContextPtr _context;
    Sprites _sprites;
};

int newMinesweeper(int argc, char **argv) {
    Mode::Enum mode = Mode::parse(argc > 1 ? *argv[1] : 'e');
    Options options{Options::getOptions(mode)};
    Layout layout{mode};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    auto window = sdl::IWindow::create("Minesweeper", layout.getWindow(), SDL_WINDOW_SHOWN);
    auto renderer = window->createRenderer();
    auto imageRepo = renderer->createImageRepo("images/");
    auto context = minesweeper::Context{window, renderer, imageRepo, layout, mode};

    auto contextPtr = std::make_shared<minesweeper::Context>(context);
    Game game(contextPtr, options);
    game.run();
    SDL_Quit();
    return 0;
}
} // namespace minesweeper
