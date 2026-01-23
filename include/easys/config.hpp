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

/**
 * @def EASYS_LOG_ENABLED
 * @brief Master switch for all logging functionality.
 * @details
 * When set to `0`, all logging code is compiled out, regardless of
 * log level or verbosity settings.
 *
 * @note Defaults to `0` (disabled).
 */
#ifndef EASYS_LOG_ENABLED
#define EASYS_LOG_ENABLED 0
#endif

/**
 * @def EASYS_LOG_VERBOSITY
 * @brief Enables additional verbose logging output.
 * @details
 * This macro can be used to enable extended or diagnostic log output
 * beyond standard severity-based filtering.
 *
 * The exact meaning of verbosity is implementation-defined.
 *
 * @note Defaults to `0` (disabled).
 */
#ifndef EASYS_LOG_VERBOSITY
#define EASYS_LOG_VERBOSITY 0
#endif

/**
 * @def EASYS_LOG_LEVEL
 * @brief Sets the global logging level.
 * @details
 * Controls which log messages are emitted based on severity.
 *
 * Valid values:
 * - `0` = NONE
 * - `1` = ERROR
 * - `2` = INFO
 * - `3` = DEBUG
 * - `4` = TRACE
 *
 * Messages with a severity greater than this level are discarded.
 *
 * @note Defaults to `2` (INFO).
 */
#ifndef EASYS_LOG_LEVEL
#define EASYS_LOG_LEVEL 2
#endif

/**
 * @def EASYS_LOG_TO_FILE
 * @brief Enables or disables logging to a file.
 * @details
 * When enabled, log output is written to the file specified by
 * ::EASYS_LOG_FILE_PATH.
 *
 * @note Defaults to `0` (disabled).
 */
#ifndef EASYS_LOG_TO_FILE
#define EASYS_LOG_TO_FILE 0
#endif

/**
 * @def EASYS_LOG_FILE_PATH
 * @brief Defines the file path used for file-based logging.
 * @details
 * This macro specifies the output file when ::EASYS_LOG_TO_FILE is enabled.
 *
 * @note Defaults to `"easys_log.txt"`.
 */
#ifndef EASYS_LOG_FILE_PATH
#define EASYS_LOG_FILE_PATH "easys_log.txt"
#endif