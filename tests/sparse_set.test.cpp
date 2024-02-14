#include <catch2/catch.hpp>
#include <ecs/sparse_set.hpp>

TEST_CASE("SparseSet functionality", "[SparseSet]")
{
	SparseSet<unsigned int, int> set;

	SECTION("Initially empty")
	{
		REQUIRE(set.size() == 0);
	}

	SECTION("Add elements")
	{
		set.add(1, 100);
		set.add(2, 200);

		REQUIRE(set.size() == 2);
	}

	SECTION("Get elements")
	{
		set.add(1, 100);
		set.add(2, 200);

		REQUIRE(set.get(1) == 100);
		REQUIRE(set.get(2) == 200);
	}

	SECTION("Check existence of elements")
	{
		set.add(1, 100);
		set.add(2, 200);

		REQUIRE(set.contains(1));
		REQUIRE(set.contains(2));
		REQUIRE_FALSE(set.contains(3));
	}

	SECTION("Remove elements")
	{
		set.add(1, 100);
		set.add(2, 200);
		set.remove(1);

		REQUIRE(set.size() == 1);
		REQUIRE_FALSE(set.contains(1));
		REQUIRE(set.contains(2));
	}

	SECTION("Throws when getting a non-existing element")
	{
		set.add(1, 100);
		REQUIRE_THROWS_AS(set.get(2), std::runtime_error);
	}

	SECTION("forEach iteration")
	{
		std::vector<unsigned int> keys;
		set.add(1, 100);
		set.add(2, 200);
		set.forEach([&keys](auto key, auto value) { keys.push_back(key); });

		REQUIRE(keys.size() == 2);
		REQUIRE(std::find(keys.begin(), keys.end(), 1) != keys.end());
		REQUIRE(std::find(keys.begin(), keys.end(), 2) != keys.end());
	}
}