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
 * @brief Evaluates ECS component types for performance, safety, and memory efficiency.
 *
 * Performs a compile-time analysis of each component in Ts and assigns a
 * practical ECS suitability grade (A–D), along with key metrics such as size,
 * alignment, cache friendliness, and relocation characteristics.
 *
 * Non-verbose mode provides a concise summary:
 * - Overall grade (A = ideal, D = problematic)
 * - Memory footprint and chunk density
 * - Key flags (e.g. memcpy-safe, relocatable, cache-friendly, pointer usage)
 * - Warnings for common ECS pitfalls
 *
 * Verbose mode includes a detailed breakdown of relevant type traits and
 * derived properties for deeper inspection.
 *
 * Core evaluation criteria:
 * - Trivially copyable & destructible (safe for bulk memory operations)
 * - Relocatability (safe movement during storage reorganization)
 * - Cache friendliness (fits within typical cache line size)
 * - Absence of raw pointers (may indicate lifetime/serialization risks)
 *
 * Notes:
 * - The analysis is heuristic-based and reflects practical ECS constraints,
 *   not strict correctness requirements.
 * - Components with pointers or non-trivial lifetimes are not inherently
 *   invalid, but require careful design consideration.
 *
 * @tparam Ts Component types to evaluate
 * @param verbose If true, prints detailed trait-level diagnostics
 */
template <typename... Ts>
void checkComponentTraits(bool verbose = false)
{
	constexpr size_t CACHE_LINE = 64;
	constexpr size_t CHUNK_SIZE = 16 * 1024;  // 16KB archetype chunk

	(
	    [&]()
	    {
		    using T = Ts;

		    printf("\n=== Component: %s ===\n", type_name<T>());

		    // ---- Core traits ----
		    constexpr size_t size = sizeof(T);
		    constexpr size_t align = alignof(T);

		    constexpr bool trivially_copyable = std::is_trivially_copyable_v<T>;
		    constexpr bool trivially_destructible = std::is_trivially_destructible_v<T>;
		    constexpr bool nothrow_move = std::is_nothrow_move_constructible_v<T>;
		    constexpr bool standard_layout = std::is_standard_layout_v<T>;

		    // Relocatability (key ECS concept)
		    constexpr bool relocatable = trivially_copyable || (nothrow_move && std::is_nothrow_destructible_v<T>);

		    // memcpy-safe (strict definition)
		    constexpr bool memcpy_safe = trivially_copyable && trivially_destructible;

		    // Heuristics
		    constexpr bool cache_friendly = size <= CACHE_LINE;
		    constexpr size_t per_chunk = size > 0 ? (CHUNK_SIZE / size) : 0;

		    // Very basic pointer detection (extend if needed)
		    constexpr bool has_pointer = std::is_pointer_v<T>;

		    // ---- Grade calculation ----
		    char grade = 'D';

		    if (memcpy_safe && cache_friendly && !has_pointer)
			    grade = 'A';
		    else if (relocatable && !has_pointer)
			    grade = 'B';
		    else if (relocatable)
			    grade = 'C';
		    else
			    grade = 'D';

		    // ---- Compact output ----
		    printf("  Grade: %c\n", grade);
		    printf("  Size: %zu bytes (align %zu)\n", size, align);
		    printf("  Chunk density: %zu per 16KB\n", per_chunk);

		    printf("  Flags: [%s%s%s%s]\n",
		           memcpy_safe ? "memcpy " : "",
		           relocatable ? "reloc " : "",
		           cache_friendly ? "cache " : "",
		           has_pointer ? "ptr " : "");

		    // Warnings (only important ones)
		    if (grade >= 'C')
		    {
			    printf("  ⚠ Potential performance issues\n");
		    }

		    if (has_pointer)
		    {
			    printf("  ⚠ Contains raw pointers (breaks relocation/serialization)\n");
		    }

		    if (!cache_friendly)
		    {
			    printf("  ⚠ Larger than cache line (%zu bytes)\n", CACHE_LINE);
		    }

		    // ---- Verbose output ----
		    if (verbose)
		    {
			    printf("\n  --- Detailed Traits ---\n");

			    printf("  trivially_copyable: %s\n", trivially_copyable ? "yes" : "no");
			    printf("  trivially_destructible: %s\n", trivially_destructible ? "yes" : "no");
			    printf("  standard_layout: %s\n", standard_layout ? "yes" : "no");

			    printf("  nothrow_move_constructible: %s\n", nothrow_move ? "yes" : "no");
			    printf("  nothrow_destructible: %s\n", std::is_nothrow_destructible_v<T> ? "yes" : "no");

			    printf("  default_constructible: %s\n", std::is_default_constructible_v<T> ? "yes" : "no");

			    printf("  copy_constructible: %s\n", std::is_copy_constructible_v<T> ? "yes" : "no");

			    printf("  move_assignable: %s\n", std::is_move_assignable_v<T> ? "yes" : "no");

			    printf("  has_unique_object_representations: %s\n",
			           std::has_unique_object_representations_v<T> ? "yes" : "no");

			    printf("\n  --- Derived Properties ---\n");

			    printf("  memcpy_safe: %s\n", memcpy_safe ? "yes" : "no");
			    printf("  relocatable: %s\n", relocatable ? "yes" : "no");
			    printf("  cache_friendly: %s\n", cache_friendly ? "yes" : "no");

			    printf("\n");
		    }
	    }(),
	    ...);
}
};  // namespace Easys