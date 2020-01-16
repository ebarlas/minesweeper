#include "../RandomFactory.h"
#include "Random.h"

namespace util {

namespace {
IRandomPtr createRandom()
{
    return std::make_shared<Random>();
}
} // namespace anonymous


RandomFactory::Factory RandomFactory::factory = createRandom;

IRandomPtr RandomFactory::create()
{
    return factory();
}

} // namespace util
