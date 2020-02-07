#include "Tile.h"

namespace minesweeper {
    Tile::Tile(ImageRepo &repo, SDL_Rect boundingBox, int adjMines, bool mine) :
            Sprite(repo, boundingBox),
            adjacentMines(adjMines),
            adjacentFlags(0),
            mine(mine),
            flagged(false),
            revealed(false),
            gameOver(false),
            flagRemaining(true) {

    }

    void Tile::setListeners(const std::vector<TileListenerWPtr> &v) {
        listeners = v;
    }

    void Tile::reset(int adjMines, bool myMine) {
        adjacentMines = adjMines;
        mine = myMine;
    }

    void Tile::onReveal(bool hasMine, bool hasAdjacentMines) {
        if (!hasMine && !hasAdjacentMines) {
            tryReveal();
        }
    }

    void Tile::onFlag(bool isFlagged) {
        if (isFlagged) {
            adjacentFlags++;
        } else {
            adjacentFlags--;
        }
    }

    void Tile::onClear() {
        tryReveal();
    }

    void Tile::handleClick(SDL_MouseButtonEvent evt) {
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

    void Tile::onStateChange(GameState gs) {
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

    void Tile::onFlagStateChange(bool exhausted) {
        flagRemaining = !exhausted;
    }

    void Tile::render() {
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

    void Tile::tryReveal() {
        if (gameOver || flagged || revealed)
            return;
        revealed = true;
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onReveal(mine, adjacentMines > 0);
    }

    void Tile::tryToggleFlag() {
        if (gameOver || revealed)
            return;
        if (!flagged && !flagRemaining)
            return;
        flagged = !flagged;
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onFlag(flagged);
    }

    void Tile::tryClear() {
        if (adjacentFlags == adjacentMines)
            for (auto &listener : listeners)
                if (auto spt = listener.lock())
                    spt->onClear();
    }
}
