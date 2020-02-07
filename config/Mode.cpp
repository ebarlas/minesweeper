#include "Mode.h"

namespace minesweeper {
    Mode::Enum Mode::parse(const char mode) {
        switch (mode) {
            case 'b':
                return Mode::BEGINNER;
            case 'i':
                return Mode::INTERMEDIATE;
            default:
                return Mode::EXPERT;
        }
    }
}