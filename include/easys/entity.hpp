#pragma once

#include <stdint.h>

// stdint.h needs to be included for uint32_t for Ubuntu, otherwise fails the build.

// Provide configurations options to to the user (?)
#ifndef ENTITY_TYPE
#define ENTITY_TYPE uint32_t
#endif

#ifndef ENTITY_LIMIT
#define ENTITY_LIMIT 10000
#endif

using Entity = ENTITY_TYPE;
const Entity MAX_ENTITIES = ENTITY_LIMIT;
