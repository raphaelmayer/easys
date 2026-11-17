#pragma once

#include <stdint.h>

#include "config.hpp"

// stdint.h needs to be included for uint32_t for Ubuntu, otherwise fails the build.

namespace Easys {

using Entity = EASYS_ENTITY_TYPE;
const Entity MAX_ENTITIES = EASYS_ENTITY_LIMIT;
const Entity NULL_ENTITY = 0;

static_assert(MAX_ENTITIES == EASYS_ENTITY_LIMIT,
              "EASYS_ENTITY_TYPE cannot represent EASYS_ENTITY_LIMIT with NULL_ENTITY=0 reserved");

}  // namespace Easys
