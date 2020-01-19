#ifndef MINESWEEPER_RANDOM_H
#define MINESWEEPER_RANDOM_H

#include <random>

namespace minesweeper {
    class Random {
    public:
        Random();
        int randomInt(int min, int max);
    private:
        std::mt19937 mersenne;
    };
};

#endif
