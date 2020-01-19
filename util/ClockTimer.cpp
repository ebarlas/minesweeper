#include "ClockTimer.h"

namespace minesweeper {
    ClockTimer::ClockTimer() : start(clock_t::now()) {
    }

    void ClockTimer::reset() {
        start = clock_t::now();
    }

    [[nodiscard]] double ClockTimer::elapsed() const {
        return std::chrono::duration_cast<second_t>(clock_t::now() - start).count();
    }
}