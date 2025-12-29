#include <catch2/catch.hpp>
#include <easys/sparse_set.hpp>

TEST_CASE("SparseSet functionality", "[SparseSet]")
{
	SECTION("Initially empty")
	{
		SparseSet<unsigned int, int> set;
		REQUIRE(set.size() == 0);
		REQUIRE(set.capacity() == 0);
	}

	SECTION("Add elements")
	{
		SparseSet<unsigned int, int> set;
		set.set(1, 100);
		set.set(2, 200);

		REQUIRE(set.size() == 2);
	}

	SECTION("Get elements")
	{
		SparseSet<unsigned int, int> set;
		set.set(1, 100);
		set.set(2, 200);

		REQUIRE(set.get(1) == 100);
		REQUIRE(set.get(2) == 200);
	}

	SECTION("Throws when getting a non-existing element")
	{
		SparseSet<unsigned int, int> set;
		set.set(1, 100);
		REQUIRE_THROWS_AS(set.get(2), KeyNotFoundException);
	}

	SECTION("Check existence of elements")
	{
		SparseSet<unsigned int, int> set;
		set.set(1, 100);
		set.set(2, 200);

		REQUIRE(set.contains(1));
		REQUIRE(set.contains(2));
		REQUIRE_FALSE(set.contains(3));
	}

	SECTION("Remove elements")
	{
		SparseSet<unsigned int, int> set;
		set.set(1, 100);
		set.set(2, 200);
		set.remove(1);

		REQUIRE(set.size() == 1);
		REQUIRE_FALSE(set.contains(1));
		REQUIRE(set.contains(2));
	}

	SECTION("forEach iteration")
	{
		SparseSet<unsigned int, int> set;
		std::vector<unsigned int> keys;
		set.set(1, 100);
		set.set(2, 200);
		set.forEach(
		    [&keys](auto key, auto value)
		    {
			    keys.push_back(key);
		    });

		REQUIRE(keys.size() == 2);
		REQUIRE(std::find(keys.begin(), keys.end(), 1) != keys.end());
		REQUIRE(std::find(keys.begin(), keys.end(), 2) != keys.end());
	}
}

TEST_CASE("SparseSet accommodate method tests", "[SparseSet]")
{
	SparseSet<unsigned int, int> set;

	SECTION("Accommodate increases size for new key")
	{
		REQUIRE(set.capacity() == 0);
		set.accommodate(10);            // Choose a key that requires resizing
		REQUIRE(set.capacity() >= 11);  // Check if sparse size is correctly adjusted
		REQUIRE(set.size() == 0);       // No elements should be added, just accommodation
	}

	SECTION("Accommodate handles maximum key value without throwing")
	{
		// TODO: this test can fail with bad allocation, fix. this test might not even be sensible.
		// REQUIRE_NOTHROW(set.accommodate(std::numeric_limits<unsigned int>::max() - 1));
	}

	SECTION("Accommodate throws length_error for key exceeding max size")
	{
		REQUIRE_THROWS_AS(set.accommodate(std::numeric_limits<unsigned int>::max()), std::length_error);
	}
}

TEST_CASE("SparseSet max_size method tests", "[SparseSet]")
{
	SparseSet<unsigned int, int> set;

	SECTION("max_size returns expected value")
	{
		size_t expectedMaxSize =
		    std::min({set.getValues().max_size(), static_cast<size_t>(std::numeric_limits<unsigned int>::max())});
		REQUIRE(set.maxSize() == expectedMaxSize);
	}

	SECTION("max_size is not greater than internal vectors' max_size")
	{
		REQUIRE(set.maxSize() <= set.getValues().max_size());
		REQUIRE(set.maxSize() <= set.getKeys().max_size());
	}
}

TEST_CASE("SparseSet size functionality", "[SparseSet]")
{
	SparseSet<unsigned, int> sparseSet;

	REQUIRE(sparseSet.size() == 0);

	sparseSet.set(1, 1);
	REQUIRE(sparseSet.size() == 1);

	sparseSet.set(5, 1);
	REQUIRE(sparseSet.size() == 2);
}

TEST_CASE("SparseSet capacity functionality", "[SparseSet]")
{
	SparseSet<unsigned, int> sparseSet;

	REQUIRE(sparseSet.capacity() == 0);

	sparseSet.accommodate(1);
	REQUIRE(sparseSet.capacity() >= 1);

	sparseSet.set(5, 1);
	REQUIRE(sparseSet.capacity() >= 5);
}

TEST_CASE("SparseSet clear functionality", "[SparseSet]")
{
	SparseSet<Entity, int> sparseSet;
	sparseSet.set(1, 1);

	SECTION("Clearing the SparseSet")
	{
		sparseSet.clear();
		REQUIRE(sparseSet.size() == 0);
		REQUIRE(sparseSet.capacity() == 0);  
	}
}