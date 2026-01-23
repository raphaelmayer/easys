#include <catch2/catch.hpp>
#include <easys/ecs.hpp>
#include <easys/entity.hpp>
#include <easys/view.hpp>

using namespace Easys;

TEST_CASE("View basic functionality", "[view]")
{
	std::vector<Entity> entities = {1, 2, 3, 4, 5};
	View view(entities);                  // copies vector
	View movedView(std::move(entities));  // moves vector

	SECTION("Size and empty")
	{
		REQUIRE(view.size() == 5);
		REQUIRE_FALSE(view.empty());

		std::vector<Entity> emptyVec;
		View emptyView(emptyVec);
		REQUIRE(emptyView.empty());
		REQUIRE(emptyView.size() == 0);
	}

	SECTION("Iteration")
	{
		size_t count = 0;
		for (auto e : view)
		{
			REQUIRE(e >= 1);
			REQUIRE(e <= 5);
			++count;
		}
		REQUIRE(count == 5);
	}

	SECTION("find() works")
	{
		auto it = view.find(3);
		REQUIRE(it != view.end());
		REQUIRE(*it == 3);

		it = view.find(42);
		REQUIRE(it == view.end());
	}

	SECTION("contains() works")
	{
		REQUIRE(view.contains(1));
		REQUIRE(view.contains(5));
		REQUIRE_FALSE(view.contains(42));
	}

	SECTION("Move constructor")
	{
		View v(std::move(view));
		REQUIRE(v.size() == 5);
	}

	SECTION("Copy constructor")
	{
		View v(view);
		REQUIRE(v.size() == 5);
		REQUIRE(v.contains(2));
	}

	SECTION("Move assignment")
	{
		View v(std::vector<Entity>{});
		v = std::move(view);
		REQUIRE(v.size() == 5);
		REQUIRE(v.contains(4));
	}

	SECTION("Copy assignment")
	{
		View v(std::vector<Entity>{});
		v = view;
		REQUIRE(v.size() == 5);
		REQUIRE(v.contains(4));
	}
}
