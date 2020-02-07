#ifndef MINESWEEPER_GRID_H
#define MINESWEEPER_GRID_H

#include "../config/Options.h"
#include "../config/Layout.h"
#include "../util/Matrix.h"
#include "Tile.h"
#include "MineField.h"

namespace minesweeper {
    class Grid : public Sprite, public GameStateListener, public FlagStateListener {
    public:
        Grid(ImageRepo &imageRepo, const Options &options, const Layout &layout);
        void setListeners(const std::vector<TileListenerWPtr> &v);
        void handleClick(SDL_MouseButtonEvent evt) override;
        void onFlagStateChange(bool exhausted) override;
        void onStateChange(GameState state) override;
        void render() override;
    private:
        Matrix<TilePtr> tiles;
        MineField mineField;
        const Options &options;
    };

    using GridPtr = std::shared_ptr<Grid>;
};

#endif
