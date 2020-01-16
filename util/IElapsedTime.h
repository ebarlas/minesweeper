#pragma once

#include <memory>

namespace util {

/**
 * IELapsedTime models a stopwatch, where you reset it and can query the current elaped time.
 */
class IElapsedTime
{
public:
    virtual ~IElapsedTime() = default;

public:
    virtual void reset() = 0;
    [[nodiscard]] virtual double elapsed() const = 0;
};

using IElapsedTimePtr = std::shared_ptr<IElapsedTime>;
using IElapsedTimeUPtr = std::unique_ptr<IElapsedTime>;

// free function for creating an elapsed time
IElapsedTimeUPtr createElapsedTime();

} // namespace util
