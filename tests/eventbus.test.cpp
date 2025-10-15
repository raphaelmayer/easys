#include <catch2/catch.hpp>
#include <easys/entity.hpp>
#include <easys/eventbus.hpp>

struct ComponentA {
	int value;
};

using TestEventbus = Eventbus<EntityAdded, EntityUpdated<ComponentA>, ComponentAdded<ComponentA>, ComponentRemoved<ComponentA>>;

TEST_CASE("Eventbus Tests", "[Eventbus]")
{
	TestEventbus bus;

	SECTION("Subscribe")
	{
		bool called = false;
		Entity entity = 42;
		
		CallbackId id = bus.subscribe<EntityAdded>(
			[&](const EntityAdded& ev) 
			{
				called = true;
				REQUIRE(ev.entity == entity);
			});
		
		bus.emit(EntityAdded(entity));
		bus.dispatch();
		REQUIRE(called);
	}

	SECTION("Eventbus - multiple subscribers reveive events", "[Eventbus]") 
	{
		int callsA = 0;
		int callsB = 0;

		bus.subscribe<EntityAdded>([&](const EntityAdded&) { callsA++; });
		bus.subscribe<EntityAdded>([&](const EntityAdded&) { callsA++;callsB++; });

		bus.emit(EntityAdded{1});
		bus.emit(EntityAdded{2});
		bus.dispatch();

		REQUIRE(callsA == 4);
		REQUIRE(callsB == 2);
	}

	SECTION("Eventbus - unsubscribe removes callbacks", "[Eventbus]") 
	{
		int calls = 0;

		auto sub = bus.subscribe<EntityAdded>([&](const EntityAdded&) { calls++; });

		bus.emit(EntityAdded{1});
		bus.dispatch();
		bus.emit(EntityAdded{2});
		bus.unsubscribe(sub);
		bus.emit(EntityAdded{3});

		bus.dispatch();

		REQUIRE(calls == 1); // only first event handled before unsubscribe
	}

	SECTION("Eventbus - dispatch clears queue", "[Eventbus]")
	{
		int calls = 0;

		bus.subscribe<EntityAdded>([&](const EntityAdded&) { calls++; });

		bus.emit(EntityAdded{1});
		bus.dispatch();
		REQUIRE(calls == 1);

		// ensure no event is left behind
		bus.dispatch();
		REQUIRE(calls == 1);
	}

	SECTION("Eventbus - different event types work independently", "[Eventbus]")
	{
		int entityAddedCalled = 0;
		int compAddedCalled = 0;

		bus.subscribe<EntityAdded>([&](const EntityAdded&) { entityAddedCalled++; });
		bus.subscribe<ComponentAdded<ComponentA>>([&](const ComponentAdded<ComponentA>& ev) { 
			REQUIRE(ev.component.value == 123);
			compAddedCalled++;
		});

		bus.emit(EntityAdded{1});
		bus.emit(ComponentAdded<ComponentA>{1, {123}});
		bus.dispatch();

		REQUIRE(entityAddedCalled == 1);
		REQUIRE(compAddedCalled == 1);
	}

	SECTION("Eventbus - subscriber snapshot isolation", "[Eventbus]")
	{
		int calls = 0;

		CallbackId sub = bus.subscribe<EntityAdded>([&](const EntityAdded&) { 
			calls++;
			bus.unsubscribe(sub);
		});

		bus.emit(EntityAdded{1});
		bus.emit(EntityAdded{2});
		bus.dispatch();

		REQUIRE(calls == 2);
	}

	SECTION("Eventbus - no crash when no subscribers or events", "[Eventbus]")
	{
		// no subs, dispatch should safely do nothing
		REQUIRE_NOTHROW(bus.dispatch());

		// emit events, but still no subs
		bus.emit(EntityAdded{1});
		REQUIRE_NOTHROW(bus.dispatch());
	}

	SECTION("Eventbus - unsubscribe with invalid id is safe", "[Eventbus]")
	{
		REQUIRE_NOTHROW(bus.unsubscribe(0));
		REQUIRE_NOTHROW(bus.unsubscribe(99999));
	}

	SECTION("Emit") 
	{
		bus.emit(EntityAdded{0});
	}

	SECTION("Dispatch") 
	{
		bus.dispatch();
	}

	SECTION("")
	{
		// bus.subscribe<ComponentUpdated<ComponentA>>([](const ComponentUpdated<ComponentA>& ev) {
		// 	if (ev.component.value <= 0)
		// 		ecs.
		// });
	}
}