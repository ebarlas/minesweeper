#include "Random.h"

namespace util {

unsigned Random::random(unsigned min, unsigned max)
{
    std::uniform_int_distribution r {min, max};
    return r(_mersenne);
}

} // namespace util
