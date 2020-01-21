#ifndef MINESWEEPER_MODE_H
#define MINESWEEPER_MODE_H

namespace minesweeper {
    class Mode {
    public:
        enum Enum {
            BEGINNER,
            INTERMEDIATE,
            EXPERT
        };

        static Mode::Enum parse(char mode);
    };
}

#endif
