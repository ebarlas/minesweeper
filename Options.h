#pragma once

#include "Mode.h"

#include <functional>

namespace minesweeper {

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

    static Options getOptions(const Mode::Enum mode) {
        switch (mode) {
            case Mode::BEGINNER:
                return Options{9, 9, 10};
            case Mode::INTERMEDIATE:
                return Options{16, 16, 40};
            default:
                return Options{16, 30, 99};
        }
    }
};

} // namespace minesweeper
