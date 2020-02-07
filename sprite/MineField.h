#ifndef MINESWEEPER_MINEFIELD_H
#define MINESWEEPER_MINEFIELD_H

#include <set>
#include "../util/Random.h"
#include "../config/Options.h"

namespace minesweeper {
    class MineField {
    public:
        explicit MineField(const Options &options);
        void reset();
        bool mineAt(int row, int col);
        int adjacentMines(int row, int col);
    private:
        Random random;
        std::set<int> mines;
        const Options &options;
    };
};

#endif
