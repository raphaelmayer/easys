#include <catch2/catch.hpp>
#include <easys/entity.hpp>
#include <easys/eventbus.hpp>

// #define COMPONENT_TYPES TestComponent, AnotherComponent
//
// struct TestComponent {
//	int value;
// };
//
// struct AnotherComponent {
//	int value;
// };

TEST_CASE("Eventbus Tests", "[Eventbus]")
{
	Eventbus<EntityAdded> evbus;
	
	std::cout << "subscribe\n";
	CallbackId sub = evbus.subscribe<EntityAdded>(
	    [](const EntityAdded& ev)// non-const ref leads to compile error
	    {
		    std::cout << "Entity from CB: " << ev.entity << "\n";
	    });

	std::cout << "unsubscribe\n";
	evbus.unsubscribe(sub);

	std::cout << "emit\n";
	evbus.emit(EntityAdded{0});

	std::cout << "dispatch\n";
	evbus.dispatch();
}