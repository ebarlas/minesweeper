#pragma once

namespace minesweeper {

class Mode {
public:
    enum Enum {
        BEGINNER,
        INTERMEDIATE,
        EXPERT
    };

    static Mode::Enum parse(const char mode) {
        switch (mode) {
            case 'b':
                return BEGINNER;
            case 'i':
                return INTERMEDIATE;
            default:
                return EXPERT;
        }
    }
};

} // namespace minesweeper
