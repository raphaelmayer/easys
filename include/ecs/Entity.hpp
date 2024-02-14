#pragma once

#include <stdint.h>

// stdint.h needs to be included for uint32_t for Ubuntu, otherwise fails the build.

using Entity = uint32_t;           // does not have to be this size/type
const Entity MAX_ENTITIES = 10000; // this value can be changed aswell
