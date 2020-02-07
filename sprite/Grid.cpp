#include "Grid.h"

namespace minesweeper {
    Grid::Grid(ImageRepo &imageRepo, const Options &options, const Layout &layout) :
            Sprite(imageRepo, layout.getGrid()),
            tiles{options.getRows(), options.getColumns()},
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

    void Grid::setListeners(const std::vector<TileListenerWPtr> &v) {
        auto fe = [&v, this](int r, int c, TilePtr &t) {
            std::vector<TileListenerWPtr> listeners = v;
            auto fn = [&listeners, this](int r, int c) { listeners.push_back(tiles.at(r, c)); };
            options.forEachNeighbor(r, c, fn);
            t->setListeners(listeners);
        };
        tiles.forEach(fe);
    }

    void Grid::handleClick(SDL_MouseButtonEvent evt) {
        int col = (evt.x - boundingBox.x) / Layout::getTileSide();
        int row = (evt.y - boundingBox.y) / Layout::getTileSide();
        tiles.at(row, col)->handleClick(evt);
    }

    void Grid::onFlagStateChange(bool exhausted) {
        tiles.forEach([exhausted](int r, int c, std::shared_ptr<Tile> &t) { t->onFlagStateChange(exhausted); });
    }

    void Grid::onStateChange(GameState state) {
        if (state == GameState::INIT) {
            mineField.reset();
            auto fn = [this](int r, int c, TilePtr &t) {
                t->reset(mineField.adjacentMines(r, c), mineField.mineAt(r, c));
            };
            tiles.forEach(fn);
        }

        tiles.forEach([state](int r, int c, std::shared_ptr<Tile> &t) { t->onStateChange(state); });
    }

    void Grid::render() {
        tiles.forEach([](int r, int c, TilePtr &t) { t->render(); });
    }
}
