#include "Options.h"

namespace minesweeper {
    Options::Options(int rows, int columns, int mines) : rows(rows), columns(columns), mines(mines) {

    }

    [[nodiscard]] int Options::getTiles() const {
        return rows * columns;
    }

    [[nodiscard]] int Options::getMines() const {
        return mines;
    }

    [[nodiscard]] int Options::getBlanks() const {
        return getTiles() - mines;
    }

    [[nodiscard]] int Options::getRows() const {
        return rows;
    }

    [[nodiscard]] int Options::getColumns() const {
        return columns;
    }

    void Options::forEachNeighbor(int row, int col, const std::function<void(int, int)> &fn) const {
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

    Options Options::getOptions(const Mode::Enum mode) {
        switch (mode) {
            case Mode::BEGINNER:
                return Options{9, 9, 10};
            case Mode::INTERMEDIATE:
                return Options{16, 16, 40};
            default:
                return Options{16, 30, 99};
        }
    }
}