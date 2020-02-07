#include "MineField.h"

namespace minesweeper {
    MineField::MineField(const Options &options) : options(options) {
        reset();
    }

    void MineField::reset() {
        mines.clear();
        for (int i = 1; i <= options.getMines(); i++) {
            while (mines.size() < i) {
                mines.insert(random.randomInt(0, options.getTiles() - 1));
            }
        }
    }

    bool MineField::mineAt(int row, int col) {
        int projection = row * options.getColumns() + col;
        return mines.find(projection) != mines.end();
    }

    int MineField::adjacentMines(int row, int col) {
        int sum = 0;
        auto fn = [&sum, this](int r, int c) { sum = sum + (mineAt(r, c) ? 1 : 0); };
        options.forEachNeighbor(row, col, fn);
        return sum;
    }
}
