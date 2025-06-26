#pragma once

// Provide configurations options for the user.

/**
 * @def EASYS_ENTITY_TYPE
 * @brief Defines the underlying data type for an Entity ID.
 * @details This can be overridden by the user to use a different integer type,
 * for example, `uint64_t` for a larger entity count. Defaults to `uint32_t`.
 */
#ifndef EASYS_ENTITY_TYPE
#define EASYS_ENTITY_TYPE uint32_t
#endif

/**
 * @def EASYS_ENTITY_LIMIT
 * @brief Defines the maximum number of entities allowed in the ECS.
 * @details This sets the limit for the total number of concurrent entities.
 * It can be overridden by the user to adjust memory usage and capacity.
 * Defaults to `10000`.
 */
#ifndef EASYS_ENTITY_LIMIT
#define EASYS_ENTITY_LIMIT 10000
#endif