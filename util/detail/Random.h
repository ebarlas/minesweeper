#pragma once

#include "../IRandom.h"
#include <random>

namespace util {

class Random : public IRandom {
public:
    Random() = default;

public:
    unsigned int random(unsigned int min, unsigned int max) override;

private:
    std::mt19937 _mersenne {static_cast<std::mt19937::result_type>(std::time(nullptr))};
};

} // namespace util
