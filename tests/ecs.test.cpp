#include <catch2/catch.hpp>
#include <easys/ecs.hpp>
#include <easys/entity.hpp>
#include <memory>

TEST_CASE("ECS Tests", "[ECS]")
{
	struct TestComponent {
		int data;
	};

	struct AnotherComponent {
		float value;
	};

	ECS ecs;

	// TODO: test constructors

	SECTION("Add Entity")
	{
		Entity entity = ecs.addEntity();
		REQUIRE(entity != -1);
		REQUIRE(ecs.getEntities().find(entity) != ecs.getEntities().end());
	}

	SECTION("Remove Entity")
	{
		Entity entity = ecs.addEntity();
		ecs.removeEntity(entity);
		REQUIRE(ecs.getEntities().find(entity) == ecs.getEntities().end());
	}

	SECTION("Has Component")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp = {10};
		ecs.addComponent<TestComponent>(entity, comp);

		REQUIRE(ecs.hasComponent<TestComponent>(entity));
		REQUIRE_FALSE(ecs.hasComponent<AnotherComponent>(entity));
	}

	SECTION("Get Component")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp = {20};
		ecs.addComponent<TestComponent>(entity, comp);

		TestComponent &retrievedComp = ecs.getComponent<TestComponent>(entity);
		REQUIRE(retrievedComp.data == 20);
	}

	SECTION("Remove Component")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp = {30};
		ecs.addComponent<TestComponent>(entity, comp);
		ecs.removeComponent<TestComponent>(entity);

		REQUIRE_FALSE(ecs.hasComponent<TestComponent>(entity));
	}

	SECTION("Component Interaction")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp1 = {40};
		AnotherComponent comp2 = {5.0f};
		ecs.addComponent<TestComponent>(entity, comp1);
		ecs.addComponent<AnotherComponent>(entity, comp2);

		REQUIRE(ecs.hasComponent<TestComponent>(entity));
		REQUIRE(ecs.hasComponent<AnotherComponent>(entity));
	}

	SECTION("Multiple Entity Handling")
	{
		Entity entity1 = ecs.addEntity();
		Entity entity2 = ecs.addEntity();
		TestComponent comp = {50};
		ecs.addComponent<TestComponent>(entity1, comp);
		ecs.addComponent<TestComponent>(entity2, comp);

		REQUIRE(ecs.hasComponent<TestComponent>(entity1));
		REQUIRE(ecs.hasComponent<TestComponent>(entity2));
	}

	SECTION("Retrieve All Entities of a Specific Component Type")
	{
		Entity entity1 = ecs.addEntity();
		Entity entity2 = ecs.addEntity();
		TestComponent comp1 = {100};
		TestComponent comp2 = {200};
		AnotherComponent comp3 = {5};
		ecs.addComponent<TestComponent>(entity1, comp1);
		ecs.addComponent<TestComponent>(entity2, comp2);
		ecs.addComponent<AnotherComponent>(entity2, comp3); // should not be included in results

		auto testComponents = ecs.getEntitiesByComponent<TestComponent>();
		REQUIRE(testComponents.size() == 2);
	}

	SECTION("Retrieve Entities by Multiple Component Types")
	{
		Entity entity1 = ecs.addEntity();
		Entity entity2 = ecs.addEntity();
		Entity entity3 = ecs.addEntity();
		TestComponent comp1 = {60};
		AnotherComponent comp2 = {10.0f};
		ecs.addComponent<TestComponent>(entity1, comp1);    // TestComponent only
		ecs.addComponent<AnotherComponent>(entity2, comp2); // AnotherComponent only
		ecs.addComponent<TestComponent>(entity3, comp1);
		ecs.addComponent<AnotherComponent>(entity3, comp2); // Both components

		struct ForeignComponent {}; // A (for the ECS) foreign component should not cause throw.

		REQUIRE(ecs.getEntitiesByComponents<TestComponent, AnotherComponent>().size() == 1);
		REQUIRE(ecs.getEntitiesByComponents<TestComponent>().size() == 2);
		REQUIRE(ecs.getEntitiesByComponents<AnotherComponent>().size() == 2);
		REQUIRE(ecs.getEntitiesByComponents<AnotherComponent, ForeignComponent>().size() == 0);
	}

	SECTION("getEntityCount returns correct number of entities", "[ECS]")
	{
		ECS ecs;

		REQUIRE(ecs.getEntityCount() == 0);

		ecs.addEntity();
		ecs.addEntity();
		REQUIRE(ecs.getEntityCount() == 2);

		ecs.addEntity();
		REQUIRE(ecs.getEntityCount() == 3);
	}

	SECTION("getComponentCount returns correct number of components", "[ECS]")
	{
		ECS ecs;
		Entity entity1 = ecs.addEntity();
		Entity entity2 = ecs.addEntity();

		REQUIRE(ecs.getComponentCount<TestComponent>() == 0);

		ecs.addComponent(entity1, TestComponent());
		REQUIRE(ecs.getComponentCount<TestComponent, AnotherComponent>() == 1);
		REQUIRE(ecs.getComponentCount() == 1);

		ecs.addComponent(entity1, AnotherComponent());
		REQUIRE(ecs.getComponentCount<TestComponent, AnotherComponent>() == 2);
		REQUIRE(ecs.getComponentCount() == 2);

		ecs.addComponent(entity2, TestComponent());
		REQUIRE(ecs.getComponentCount<TestComponent, AnotherComponent>() == 3);
		REQUIRE(ecs.getComponentCount() == 3);

		ecs.addComponent(entity2, AnotherComponent());
		REQUIRE(ecs.getComponentCount<TestComponent, AnotherComponent>() == 4);
		REQUIRE(ecs.getComponentCount() == 4);
	}

	SECTION("Clearing ECS")
	{
		ECS ecs;
		auto entity = ecs.addEntity();
		ecs.addComponent<TestComponent>(entity, TestComponent());
		ecs.addComponent<AnotherComponent>(entity, AnotherComponent());

		ecs.clearComponents<TestComponent, AnotherComponent>();
		REQUIRE(ecs.getEntityCount() == 1);
		REQUIRE(ecs.getComponentCount<TestComponent, AnotherComponent>() == 0);
	}

	SECTION("Clearing ECS")
	{
		ECS ecs;
		auto entity = ecs.addEntity();
		ecs.addComponent<TestComponent>(entity, TestComponent());
		ecs.addComponent<AnotherComponent>(entity, AnotherComponent());

		ecs.clear();
		REQUIRE(ecs.getEntityCount() == 0);
		REQUIRE(ecs.addEntity() == 0); // Check if all entity IDs are available again
		REQUIRE(ecs.getComponentCount<TestComponent, AnotherComponent>() == 0);
	}
}