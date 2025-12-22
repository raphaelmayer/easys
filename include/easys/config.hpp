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

// TODO: doxygen comments
// Logging configuration macros - override these before including
#ifndef EASYS_LOG_LEVEL
#define EASYS_LOG_LEVEL 2  // Default: INFO (0=NONE, 1=ERROR, 2=INFO, 3=DEBUG, 4=TRACE)
#endif

#ifndef EASYS_LOG_TO_FILE
#define EASYS_LOG_TO_FILE 0  // Default: disabled
#endif

#ifndef EASYS_LOG_FILE_PATH
#define EASYS_LOG_FILE_PATH "easys_log.txt"
#endif

#ifndef EASYS_LOG_VERBOSITY
#define EASYS_LOG_VERBOSITY 0 // Default: disabled
#endif

#ifndef EASYS_LOG_ENABLED
#define EASYS_LOG_ENABLED 1  // Master switch
#endif