#include <catch2/catch.hpp>
#include <easys/entity.hpp>
#include <easys/sparse_set.hpp>

using Entity = Easys::Entity;

TEST_CASE("SparseSet basic functionality", "[SparseSet]")
{
	SECTION("Initially empty")
	{
		Easys::SparseSet<Entity, int> set;
		REQUIRE(set.size() == 0);
		REQUIRE(set.empty());
	}

	SECTION("Add elements (emplace/set)")
	{
		Easys::SparseSet<Entity, int> set;
		set.set(1, 100);
		set.emplace(2, 200);

		REQUIRE(set.size() == 2);
		REQUIRE(set.contains(1));
		REQUIRE(set.contains(2));
	}

	SECTION("Safe Access (try_get)")
	{
		Easys::SparseSet<Entity, int> set;
		set.set(1, 100);

		int* val = set.try_get(1);
		REQUIRE(val != nullptr);
		REQUIRE(*val == 100);

		REQUIRE(set.try_get(999) == nullptr);
	}

	SECTION("Fast Access (operator[])")
	{
		Easys::SparseSet<Entity, int> set;
		set.set(1, 100);

		// Note: We only test valid keys because invalid ones trigger assert/UB
		REQUIRE(set[1] == 100);

		set[1] = 50;
		REQUIRE(set[1] == 50);
	}

	SECTION("Remove elements")
	{
		Easys::SparseSet<Entity, int> set;
		set.set(1, 100);
		set.set(2, 200);
		set.set(3, 300);

		set.remove(2);

		REQUIRE(set.size() == 2);
		REQUIRE(set.contains(1));
		REQUIRE_FALSE(set.contains(2));
		REQUIRE(set.contains(3));

		REQUIRE(set[3] == 300);
	}
}

TEST_CASE("SparseSet Iteration", "[SparseSet]")
{
	Easys::SparseSet<Entity, int> set;
	set.set(1, 10);
	set.set(2, 20);
	set.set(3, 30);

	SECTION("Standard range-based for")
	{
		std::vector<int> results;
		for (const auto& val : set)
		{
			results.push_back(val);
		}

		REQUIRE(results.size() == 3);
		REQUIRE(results[0] == 10);
		REQUIRE(results[1] == 20);
		REQUIRE(results[2] == 30);
	}

	SECTION("Modifying during iteration")
	{
		for (auto& val : set)
		{
			val += 1;
		}
		REQUIRE(set[1] == 11);
		REQUIRE(set[2] == 21);
	}
}

TEST_CASE("SparseSet Capacity Logic", "[SparseSet]")
{
	Easys::SparseSet<Entity, int> set;

	SECTION("Accommodate")
	{
		// Accommodate only resizes 'sparse', not 'dense' (size)
		set.accommodate(100);
		REQUIRE(set.size() == 0);
		REQUIRE(set.extent() > 100);  // Check sparse size
	}

	SECTION("Clear")
	{
		set.set(1, 10);
		set.set(2, 20);

		size_t oldSparseCapacity = set.extent();

		set.clear();

		REQUIRE(set.size() == 0);
		REQUIRE(set.empty());
		// Sparse array should NOT shrink to save allocations
		REQUIRE(set.extent() == oldSparseCapacity);
		REQUIRE_FALSE(set.contains(1));
	}
}