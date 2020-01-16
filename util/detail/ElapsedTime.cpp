#include "ElapsedTime.h"

namespace util {

void ElapsedTime::reset()
{
    _start = clock_t::now();
}

double ElapsedTime::elapsed() const
{
    return std::chrono::duration_cast<second_t>(clock_t::now() - _start).count();
}

IElapsedTimeUPtr createElapsedTime()
{
    return std::make_unique<ElapsedTime>();
}

} // namespace util
