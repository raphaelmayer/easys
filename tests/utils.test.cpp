#include <catch2/catch.hpp>
#include <easys/ecs.hpp>
#include <easys/entity.hpp>
#include <easys/utils.hpp>
#include <memory>

using namespace Easys;

#define ECS_TEST_COMPTYPES TestComponent, AnotherComponent

TEST_CASE("Utils Tests", "[Utils]")
{
	struct TestComponent {
		int data;
	};

	struct AnotherComponent {
		float value;
	};

	ECS<ECS_TEST_COMPTYPES> ecs;

	SECTION("Check component type traits")
	{
		checkComponentTraits<ECS_TEST_COMPTYPES>();
		checkComponentTraits<ECS_TEST_COMPTYPES>(true);
		REQUIRE(true);
	}
}