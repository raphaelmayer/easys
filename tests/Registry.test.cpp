#include <catch2/catch.hpp>
#include <ecs/registry.hpp>

#define COMPONENT_TYPES TestComponent, AnotherComponent

struct TestComponent {
	int value;
};

struct AnotherComponent {
	int value;
};

TEST_CASE("Registry Tests", "[Registry]")
{
	Registry registry;
	Entity testEntity = 1;

	SECTION("Add and Check Component")
	{
		TestComponent comp = {10};
		registry.addComponent<TestComponent>(testEntity, comp);

		REQUIRE(registry.hasComponent<TestComponent>(testEntity));
	}

	SECTION("Get Component")
	{
		TestComponent comp = {20};
		registry.addComponent<TestComponent>(testEntity, comp);

		TestComponent &retrievedComp = registry.getComponent<TestComponent>(testEntity);
		REQUIRE(retrievedComp.value == 20);
	}

	SECTION("Remove Component")
	{
		TestComponent comp = {10};
		AnotherComponent comp2 = {20};
		registry.addComponent<TestComponent>(testEntity, comp);
		registry.addComponent<AnotherComponent>(testEntity, comp2);
		registry.removeComponent<TestComponent>(testEntity);

		REQUIRE_FALSE(registry.hasComponent<TestComponent>(testEntity));
		REQUIRE(registry.hasComponent<AnotherComponent>(testEntity));
	}

	SECTION("Remove Components")
	{
		TestComponent comp = {10};
		AnotherComponent comp2 = {20};
		registry.addComponent<TestComponent>(testEntity, comp);
		registry.addComponent<AnotherComponent>(testEntity, comp2);
		registry.removeComponents(testEntity);

		std::cout << "removeComponents(Entity) is not implemented." << std::endl;
		return;

		REQUIRE_FALSE(registry.hasComponent<TestComponent>(testEntity));
		REQUIRE_FALSE(registry.hasComponent<AnotherComponent>(testEntity));
	}

	SECTION("ComponentType Does Not Exist")
	{
		REQUIRE_FALSE(registry.hasComponent<TestComponent>(testEntity));
	}

	SECTION("Registry size method returns total number of components")
	{
		Registry registry;

		// REQUIRE(registry.size<COMPONENT_TYPES>() == 0);

		Entity entity1 = 1, entity2 = 2, entity3 = 3;
		TestComponent component1{10};
		AnotherComponent component2{20};
		registry.addComponent<TestComponent>(entity1, component1);
		registry.addComponent<AnotherComponent>(entity2, component2);
		REQUIRE(registry.size<COMPONENT_TYPES>() == 2);

		registry.addComponent<TestComponent>(entity2, component1);
		REQUIRE(registry.size<COMPONENT_TYPES>() == 3);

		registry.addComponent<TestComponent>(entity3, component1);
		REQUIRE(registry.size<COMPONENT_TYPES>() == 4);
	}

	SECTION("Registry size method with single component type")
	{
		Registry registry;
		Entity entity = 1;
		TestComponent comp{0};
		registry.addComponent<TestComponent>(entity, comp);
		REQUIRE(registry.size<TestComponent>() == 1);
	}

	SECTION("Registry size method with no known component types")
	{
		Registry registry;
		REQUIRE(registry.size<COMPONENT_TYPES>() == 0);
	}

	SECTION("Registry size method with one unknown component type")
	{
		Registry registry;
		Entity entity = 1;
		TestComponent comp{0};
		registry.addComponent<TestComponent>(entity, comp);
		REQUIRE(registry.size<COMPONENT_TYPES>() == 1);
	}

	SECTION("Get all entities of ComponentType")
	{
		TestComponent comp1 = {10};
		TestComponent comp2 = {20};
		AnotherComponent comp3 = {30};
		Entity entity1 = 1, entity2 = 2, entity3 = 3;

		registry.addComponent<TestComponent>(entity1, comp1);
		registry.addComponent<TestComponent>(entity2, comp2);
		registry.addComponent<AnotherComponent>(entity3, comp3); // Different type, should not be included

		auto &testComponents = registry.getEntitiesByType<TestComponent>();

		REQUIRE(testComponents.size() == 2);
		REQUIRE(testComponents[0] == entity1);
		REQUIRE(testComponents[1] == entity2);
	}

	SECTION("Get All Components of ComponentType")
	{
		TestComponent comp1 = {10};
		TestComponent comp2 = {20};
		AnotherComponent comp3 = {30};
		Entity entity1 = 1, entity2 = 2, entity3 = 3;

		registry.addComponent<TestComponent>(entity1, comp1);
		registry.addComponent<TestComponent>(entity2, comp2);
		registry.addComponent<AnotherComponent>(entity3, comp3); // Different type, should not be included

		auto &testComponents = registry.getComponentsByType<TestComponent>();

		REQUIRE(testComponents.size() == 2);
		bool foundComp1 = false, foundComp2 = false;
		for (auto &comp : testComponents) {
			if (comp.value == 10)
				foundComp1 = true;
			if (comp.value == 20)
				foundComp2 = true;
		}

		REQUIRE(foundComp1);
		REQUIRE(foundComp2);

		// Verify that components of other types are not mistakenly included
		auto &anotherComponents = registry.getComponentsByType<AnotherComponent>();
		REQUIRE(anotherComponents.size() == 1);
	}
}
