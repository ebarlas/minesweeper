#ifndef MINESWEEPER_CLOCKTIMER_H
#define MINESWEEPER_CLOCKTIMER_H

#include <chrono>

namespace minesweeper {
    class ClockTimer {
    public:
        ClockTimer();
        void reset();
        [[nodiscard]] double elapsed() const;
    private:
        using clock_t = std::chrono::high_resolution_clock;
        using second_t = std::chrono::duration<double, std::ratio<1> >;
        std::chrono::time_point<clock_t> start;
    };
}

#endif
