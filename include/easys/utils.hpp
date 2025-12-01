#pragma once

#include <cstdio>
#include <type_traits>

namespace {
// Helper to get type name without typeid().name() mangling.
template <typename T>
const char* type_name()
{
#ifdef __clang__
	static const char* name = __PRETTY_FUNCTION__ + 33;
	name[strlen(name) - 1] = '\0';  // Remove trailing ']'
#elif defined(__GNUC__)
	static const char* name = __PRETTY_FUNCTION__ + 49;
	name[strlen(name) - 1] = '\0';  // Remove trailing ']'
#else
	// Fallback to typeid
	static std::string name = typeid(T).name();
	return name.c_str();
#endif
}
};  // namespace

namespace Easys {
/**
 * @brief Validates component types for ECS (Entity Component System) compatibility and performance.
 *
 * Analyzes compile-time traits of all components Ts.
 * When verbose, shows detailed type traits; otherwise shows only
 * size, alignment, and optimization status.
 *
 * Critical checks:
 * - Trivially copyable (memcpy moves between archetypes)
 * - Trivially destructible (batch destruction)
 * - Standard layout (predictable memory layout)
 * - Reasonable size/alignment (cache efficiency)
 *
 * @tparam Ts Component types to validate
 * @param verbose Enable detailed diagnostic output
 */
template <typename... Ts>
void checkComponentTraits(bool verbose = false)
{
	(
	    [&]()
	    {
		    using T = Ts;
		    printf("\n=== Checking component: %s ===\n", type_name<T>());

		    // Memory layout and copying
		    printf("  Size: %zu bytes, Alignment: %zu\n", sizeof(T), alignof(T));

		    if (verbose)
		    {
			    printf("  is_trivial: %s\n", std::is_trivial_v<T> ? "yes" : "no");
			    printf("  is_trivially_copyable: %s\n", std::is_trivially_copyable_v<T> ? "yes" : "no");
			    printf("  is_trivially_destructible: %s\n", std::is_trivially_destructible_v<T> ? "yes" : "no");

			    // Construction/destruction
			    printf("  is_default_constructible: %s\n", std::is_default_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_default_constructible: %s\n",
			           std::is_nothrow_default_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_copy_constructible: %s\n", std::is_nothrow_copy_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_move_constructible: %s\n", std::is_nothrow_move_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_destructible: %s\n", std::is_nothrow_destructible_v<T> ? "yes" : "no");

			    // Assignment
			    printf("  is_copy_assignable: %s\n", std::is_copy_assignable_v<T> ? "yes" : "no");
			    printf("  is_move_assignable: %s\n", std::is_move_assignable_v<T> ? "yes" : "no");
			    printf("  is_nothrow_copy_assignable: %s\n", std::is_nothrow_copy_assignable_v<T> ? "yes" : "no");
			    printf("  is_nothrow_move_assignable: %s\n", std::is_nothrow_move_assignable_v<T> ? "yes" : "no");

			    // Other important traits
			    printf("  is_empty: %s (can use empty base optimization)\n", std::is_empty_v<T> ? "yes" : "no");
			    printf("  is_standard_layout: %s (memcpy safe)\n", std::is_standard_layout_v<T> ? "yes" : "no");
			    printf("  is_pod (deprecated): %s\n", std::is_pod_v<T> ? "yes" : "no");
			    printf("  has_unique_object_representations: %s (hashing)\n",
			           std::has_unique_object_representations_v<T> ? "yes" : "no");

			    // For tagged components
			    printf("  is_final: %s\n", std::is_final_v<T> ? "yes" : "no");

			    // Check if type is suitable for memcpy
			    constexpr bool can_memcpy = std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>;
			    printf("  Can use memcpy for bulk operations: %s\n", can_memcpy ? "yes" : "no");
		    }

		    // Summary for quick reading
		    constexpr bool optimal_for_ecs = std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>
		                                     && std::is_nothrow_move_constructible_v<T>
		                                     && std::is_nothrow_move_assignable_v<T>
		                                     && std::is_default_constructible_v<T>;
		    printf("  Optimal for ECS: %s\n", optimal_for_ecs ? "YES" : "NO");

		    if (!optimal_for_ecs)
		    {
			    printf("  WARNING: This component may have performance overhead!\n");
		    }
	    }(),
	    ...);
}
};  // namespace Easys