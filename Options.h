#ifndef MINESWEEPER_OPTIONS_H
#define MINESWEEPER_OPTIONS_H

#include <functional>
#import "Mode.h"

namespace minesweeper {
    class Options {
    public:
        Options(int rows, int columns, int mines);
        [[nodiscard]] int getTiles() const;
        [[nodiscard]] int getMines() const;
        [[nodiscard]] int getBlanks() const;
        [[nodiscard]] int getRows() const;
        [[nodiscard]] int getColumns() const;
        void forEachNeighbor(int row, int col, const std::function<void(int, int)> &fn) const;
        static Options getOptions(const Mode::Enum mode);
    private:
        const int rows;
        const int columns;
        const int mines;
    };
};

#endif
