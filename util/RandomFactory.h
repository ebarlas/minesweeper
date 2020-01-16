#pragma once

#include <functional>
#include "IRandom.h"

namespace util {

/**
 * Random factory is a factory for creating an interface for obtaining random numbers.
 * By default, the interface is based upon the Random class. This can be changed by
 * assigning a different Factory functor to the class static factory member.
 */
class RandomFactory
{
public:
    // functor definition for obtaining an IRandom object
    using Factory = std::function<IRandomPtr()>;

public:
    // create and return an IRandom object
    static IRandomPtr create();

public:
    // the functor which create() will invoke
    static Factory  factory;
};

} // namespace util
