#pragma once

#include <stdint.h>

// stdint.h needs to be included for uint32_t, because of a bug on Ubuntu,
// which otherwise fails the build and also our CI/CD pipeline.

using Entity = uint32_t;           // does not have to be this size/type
const Entity MAX_ENTITIES = 10000; // this value can be changed aswell
