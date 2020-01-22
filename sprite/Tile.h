#ifndef MINESWEEPER_TILE_H
#define MINESWEEPER_TILE_H

#include <vector>
#include "Sprite.h"
#include "TileListener.h"
#include "GameStateListener.h"
#include "FlagStateListener.h"

namespace minesweeper {
    class Tile : public Sprite, public TileListener, public GameStateListener, public FlagStateListener {
    public:
        Tile(ImageRepo &repo, SDL_Rect boundingBox, int adjMines, bool mine);
        void setListeners(const std::vector<TileListenerWPtr> &v);
        void reset(int adjMines, bool myMine);
        void onReveal(bool hasMine, bool hasAdjacentMines) override;
        void onFlag(bool isFlagged) override;
        void onClear() override;
        void handleClick(SDL_MouseButtonEvent evt) override;
        void onStateChange(GameState gs) override;
        void onFlagStateChange(bool exhausted) override;
        void render() override;
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
        void tryReveal();
        void tryToggleFlag();
        void tryClear();
    };

    using TilePtr = std::shared_ptr<Tile>;
    using TileWPtr = std::weak_ptr<Tile>;
};

#endif
