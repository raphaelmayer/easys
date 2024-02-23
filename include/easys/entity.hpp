#pragma once

#include "config.hpp"
#include <stdint.h>

// stdint.h needs to be included for uint32_t for Ubuntu, otherwise fails the build.

using Entity = EASYS_ENTITY_TYPE;
const Entity MAX_ENTITIES = EASYS_ENTITY_LIMIT;
 