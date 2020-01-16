#pragma once

#include <memory>

namespace util {

class IRandom {
public:
    virtual ~IRandom() = default;

public:
    virtual unsigned random(unsigned min, unsigned max) = 0;
};

using IRandomPtr = std::shared_ptr<IRandom>;
using IRandomUPtr = std::unique_ptr<IRandom>;

} // namespace util
