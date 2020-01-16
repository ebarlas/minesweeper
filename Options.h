#pragma once

#include "Mode.h"

#include <functional>

namespace minesweeper {

class Options {
public:
    Options(unsigned rows, unsigned columns, unsigned mines) : rows(rows), columns(columns), mines(mines) {

    }

    [[nodiscard]] unsigned getTiles() const {
        return rows * columns;
    }

    [[nodiscard]] unsigned getMines() const {
        return mines;
    }

    [[nodiscard]] unsigned getBlanks() const {
        return getTiles() - mines;
    }

    [[nodiscard]] unsigned getRows() const {
        return rows;
    }

    [[nodiscard]] unsigned getColumns() const {
        return columns;
    }

    void forEachNeighbor(unsigned row, unsigned col, const std::function<void(unsigned, unsigned)> &fn) const {
        for (auto r = row - 1; r <= row + 1; r++) {
            for (auto c = col - 1; c <= col + 1; c++) {
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

private:
    const unsigned rows;
    const unsigned columns;
    const unsigned mines;

};

} // namespace minesweeper
