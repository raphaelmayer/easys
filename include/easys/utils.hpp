#pragma once

#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>

namespace {
#if defined(__GNUG__) || defined(__clang__)
#include <cxxabi.h>

#include <cstdlib>
#endif

// get a compiler-provided type name (no UB)
template <typename T>
std::string_view raw_type_name()
{
#if defined(__clang__)
	// std::string_view raw_type_name() [T = Foo]
	std::string_view p = __PRETTY_FUNCTION__;
	auto start = p.find("T = ") + 4;
	auto end = p.rfind(']');
	return p.substr(start, end - start);

#elif defined(__GNUC__)
	// std::string_view raw_type_name() [with T = Foo; ...]
	std::string_view p = __PRETTY_FUNCTION__;
	auto start = p.find("T = ") + 4;
	auto end = p.find(';', start);
	return p.substr(start, end - start);

#elif defined(_MSC_VER)
	// class std::basic_string_view<...> __cdecl raw_type_name<Foo>(void)
	std::string_view p = __FUNCSIG__;
	auto start = p.find("raw_type_name<") + 14;
	auto end = p.find(">(void)", start);
	return p.substr(start, end - start);

#else
	return typeid(T).name();
#endif
}

// demangle if needed (mainly MSVC / fallback)
inline std::string demangle(std::string_view sv)
{
#if defined(__GNUG__) || defined(__clang__)
	int status = 0;
	char* p = abi::__cxa_demangle(sv.data(), nullptr, nullptr, &status);
	std::string result = (status == 0 && p) ? p : std::string(sv);
	std::free(p);
	return result;
#else
	return std::string(sv);
#endif
}

// Helper to print a prettier type name
template <typename T>
const char* type_name()
{
	// static std::string name = normalize_type_name(raw_type_name<T>());
	static std::string name = std::string(demangle(raw_type_name<T>()));
	return name.c_str();
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
				printf("  is_standard_layout: %s (memcpy safe)\n", std::is_standard_layout_v<T> ? "yes" : "no");
			    printf("  is_default_constructible: %s\n", std::is_default_constructible_v<T> ? "yes" : "no");
			    printf("  is_copy_assignable: %s\n", std::is_copy_assignable_v<T> ? "yes" : "no");
			    printf("  is_move_assignable: %s\n", std::is_move_assignable_v<T> ? "yes" : "no");
				
				// nothrow
				printf("  is_nothrow_default_constructible: %s\n",
			           std::is_nothrow_default_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_copy_constructible: %s\n", std::is_nothrow_copy_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_move_constructible: %s\n", std::is_nothrow_move_constructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_destructible: %s\n", std::is_nothrow_destructible_v<T> ? "yes" : "no");
			    printf("  is_nothrow_copy_assignable: %s\n", std::is_nothrow_copy_assignable_v<T> ? "yes" : "no");
			    printf("  is_nothrow_move_assignable: %s\n", std::is_nothrow_move_assignable_v<T> ? "yes" : "no");
				
			    // Other important traits
			    printf("  is_empty: %s (can use empty base optimization)\n", std::is_empty_v<T> ? "yes" : "no");
			    // printf("  is_pod (deprecated): %s\n", std::is_pod_v<T> ? "yes" : "no");
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