#include <ecs/VectorRegistry.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Registry Tests", "[Registry]")
{
	struct TestComponent {
		int value;
	};

	struct AnotherComponent {
		int value;
	};

	Registry registry;
	Entity testEntity = 1;

	SECTION("Add and Check Component")
	{
		TestComponent comp = { 10 };
		registry.addComponent<TestComponent>(testEntity, comp);

		REQUIRE(registry.hasComponent<TestComponent>(testEntity));
	}

	SECTION("Get Component")
	{
		TestComponent comp = { 20 };
		registry.addComponent<TestComponent>(testEntity, comp);

		TestComponent& retrievedComp = registry.getComponent<TestComponent>(testEntity);
		REQUIRE(retrievedComp.value == 20);
	}

	SECTION("Remove Component")
	{
		TestComponent comp = { 10 };
		AnotherComponent comp2 = { 20 };
		registry.addComponent<TestComponent>(testEntity, comp);
		registry.addComponent<AnotherComponent>(testEntity, comp2);
		registry.removeComponent<TestComponent>(testEntity);

		REQUIRE_FALSE(registry.hasComponent<TestComponent>(testEntity));
		REQUIRE(registry.hasComponent<AnotherComponent>(testEntity));
	}

	SECTION("Remove Components")
	{
		TestComponent comp = { 10 };
		AnotherComponent comp2 = { 20 };
		registry.addComponent<TestComponent>(testEntity, comp);
		registry.addComponent<AnotherComponent>(testEntity, comp2);
		registry.removeComponents(testEntity);

		REQUIRE_FALSE(registry.hasComponent<TestComponent>(testEntity));
		REQUIRE_FALSE(registry.hasComponent<AnotherComponent>(testEntity));
	}

	SECTION("Component Does Not Exist")
	{
		REQUIRE_FALSE(registry.hasComponent<TestComponent>(testEntity));
	}

	SECTION("Registry size method returns total number of components", "[Registry]")
	{
		Registry registry;

		REQUIRE(registry.size() == 0);

		Entity entity1 = 1, entity2 = 2, entity3 = 3;
		TestComponent component1{ 10 };
		AnotherComponent component2{ 20 };
		registry.addComponent<TestComponent>(entity1, component1);
		registry.addComponent<AnotherComponent>(entity2, component2);
		REQUIRE(registry.size() == 2);

		registry.addComponent<TestComponent>(entity2, component1);
		REQUIRE(registry.size() == 3);

		registry.addComponent<TestComponent>(entity3, component1);
		REQUIRE(registry.size() == 4);
	}

	SECTION("Get All Components of Type")
	{
		// Setup: Add multiple components of the same type to different entities
		TestComponent comp1 = { 10 };
		TestComponent comp2 = { 20 };
		AnotherComponent comp3 = { 30 };
		Entity entity1 = 1, entity2 = 2, entity3 = 3;

		registry.addComponent<TestComponent>(entity1, comp1);
		registry.addComponent<TestComponent>(entity2, comp2);
		registry.addComponent<AnotherComponent>(entity3, comp3); // Different type, should not be included

		// Act: Retrieve all TestComponent instances
		auto testComponents = registry.getAllComponentsByType<TestComponent>();

		// Assert: Verify correct components are retrieved and their values
		REQUIRE(testComponents.size() == 2);
		bool foundComp1 = false, foundComp2 = false;
		for (auto& compRef : testComponents) {
			TestComponent& comp = compRef.get();
			if (comp.value == 10)
				foundComp1 = true;
			if (comp.value == 20)
				foundComp2 = true;
		}

		REQUIRE(foundComp1);
		REQUIRE(foundComp2);

		// Verify that components of other types are not mistakenly included
		auto anotherComponents = registry.getAllComponentsByType<AnotherComponent>();
		REQUIRE(anotherComponents.size() == 1);
	}
}
