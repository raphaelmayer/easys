#include <catch2/catch.hpp>
#include <memory>
#include <ECSManager.hpp>

TEST_CASE("ECSManager Tests", "[ECSManager]")
{
	struct TestComponent {
		int data;
	};

	struct AnotherComponent {
		float value;
	};

	ECSManager ecs;

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

	SECTION("Add and Check Component")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp = { 10 };
		ecs.addComponent<TestComponent>(entity, comp);

		REQUIRE(ecs.hasComponent<TestComponent>(entity));
	}

	SECTION("Get Component")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp = { 20 };
		ecs.addComponent<TestComponent>(entity, comp);

		TestComponent& retrievedComp = ecs.getComponent<TestComponent>(entity);
		REQUIRE(retrievedComp.data == 20);
	}

	SECTION("Retrieve All Components of a Specific Type")
	{
		Entity entity1 = ecs.addEntity();
		Entity entity2 = ecs.addEntity();
		TestComponent comp1 = { 100 };
		TestComponent comp2 = { 200 };
		//AnotherComponent comp3 = {5};
		ecs.addComponent<TestComponent>(entity1, comp1);
		ecs.addComponent<TestComponent>(entity2, comp2);
		//ecs.addComponent<AnotherComponent>(entity2, comp3);

		auto testComponents = ecs.getAllComponentsByType<TestComponent>();
		REQUIRE(testComponents.size() == 2);
	}

	SECTION("Remove Component")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp = { 30 };
		ecs.addComponent<TestComponent>(entity, comp);
		ecs.removeComponent<TestComponent>(entity);

		REQUIRE_FALSE(ecs.hasComponent<TestComponent>(entity));
	}

	SECTION("Component Interaction")
	{
		Entity entity = ecs.addEntity();
		TestComponent comp1 = { 40 };
		AnotherComponent comp2 = { 5.0f };
		ecs.addComponent<TestComponent>(entity, comp1);
		ecs.addComponent<AnotherComponent>(entity, comp2);

		REQUIRE(ecs.hasComponent<TestComponent>(entity));
		REQUIRE(ecs.hasComponent<AnotherComponent>(entity));
	}

	SECTION("Multiple Entity Handling")
	{
		Entity entity1 = ecs.addEntity();
		Entity entity2 = ecs.addEntity();
		TestComponent comp = { 50 };
		ecs.addComponent<TestComponent>(entity1, comp);
		ecs.addComponent<TestComponent>(entity2, comp);

		REQUIRE(ecs.hasComponent<TestComponent>(entity1));
		REQUIRE(ecs.hasComponent<TestComponent>(entity2));
	}

	SECTION("getEntityCount returns correct number of entities", "[ECSManager]")
	{
		ECSManager ecs;

		REQUIRE(ecs.getEntityCount() == 0);

		ecs.addEntity();
		ecs.addEntity();
		REQUIRE(ecs.getEntityCount() == 2);

		ecs.addEntity();
		REQUIRE(ecs.getEntityCount() == 3);
	}

	SECTION("getComponentCount returns correct number of components", "[ECSManager]")
	{
		ECSManager ecs;
		Entity entity = ecs.addEntity();

		REQUIRE(ecs.getComponentCount() == 0);

		ecs.addComponent<TestComponent>(entity, TestComponent());
		REQUIRE(ecs.getComponentCount() == 1);
		ecs.addComponent<AnotherComponent>(entity, AnotherComponent());
		REQUIRE(ecs.getComponentCount() == 2);
	}
}