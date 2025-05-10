#pragma once

#include <stdint.h>

#include "config.hpp"

// stdint.h needs to be included for uint32_t for Ubuntu, otherwise fails the build.

namespace Easys {

using Entity = EASYS_ENTITY_TYPE;
const Entity MAX_ENTITIES = EASYS_ENTITY_LIMIT;

}  // namespace Easys
