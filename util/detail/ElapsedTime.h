#pragma once

#include "../IElapsedTime.h"

#include <chrono>

namespace util {

/**
 * ELapsedTime implementation
 */
class ElapsedTime: public IElapsedTime
{
public:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

public:
    ElapsedTime() = default;

public:
    void reset() override;
    double elapsed() const override;

private:
    std::chrono::time_point<clock_t> _start = clock_t::now();
};

} // namespace util
