#include "Random.h"

namespace minesweeper {
    Random::Random() : mersenne{static_cast<std::mt19937::result_type>(std::time(nullptr))} {

    }

    int Random::randomInt(int min, int max) {
        std::uniform_int_distribution die{min, max};
        return die(mersenne);
    }
}
