#pragma once

#include "Mode.h"
#include "util/Coordinates.h"

#include <functional>

namespace minesweeper {

class Options {
public:
    using ordinate_type = util::Coordinate::ordinate_type;
    using Coordinate = util::Coordinate;

public:
    Options(ordinate_type rows, ordinate_type columns, unsigned mines) : _coords{rows, columns}, mines(mines) {

    }

public:
    [[nodiscard]] auto getTiles() const {
        return _coords.rows() * _coords.columns();
    }

    [[nodiscard]] auto getMines() const {
        return mines;
    }

    [[nodiscard]] auto getBlanks() const {
        return getTiles() - mines;
    }

    [[nodiscard]] auto getRows() const {
        return _coords.rows();
    }

    [[nodiscard]] auto getColumns() const {
        return _coords.columns();
    }

    auto neighbors(ordinate_type row, ordinate_type col) const { return _coords.neighbors(row, col); }
    auto neighbors(const Coordinate& coord) const { return _coords.neighbors(coord); }
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
    const util::Coordinates _coords;
    const unsigned mines;

};

} // namespace minesweeper
