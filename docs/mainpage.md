\mainpage Easys::ECS Documentation

\brief

EasyS is a minimalist, header-only C++ library designed to streamline the development of applications using the Entity Component System (ECS) architecture. With a focus on simplicity, flexibility, and ease of use, it offers developers an unopinionated foundation to build efficient, high-performance systems without the overhead of external dependencies.  

**The library uses a static design approach**, requiring all component types to be registered at compile time through template parameters.  

\tableofcontents

## Easys::ECS

The \ref Easys::ECS class is the single public interface for EasyS. It provides all functionality for:

- Compile-time component registration  
- Entity creation and management  
- Component addition, removal, and querying  

## Quick Start Guide

This basic example demonstrates how to create entities, add components, and retrieve them:
```
#include <easys/easys.hpp>
#include <iostream>

// This is a very basic example to demonstrate creating an entity,
// adding a component to it, and retrieving the component.

// Define a Position component structure.
// Components are simple structs or classes that contain data.
struct Position {
	float x, y;
};

int main()
{
	using namespace Easys;
  
	// Create an instance of the ECS class and register all component types.
	ECS<Position> ecs;

	// Create a new entity. An entity is just a number.
	Entity entity = ecs.addEntity();

	// Add a Position component to the entity with initial values.
	ecs.addComponent<Position>(entity, {10.0f, 20.0f});

	// Update the component.
	auto &component = ecs.getComponent<Position>(entity);
	component.x = 0.0f;

	// Retrieve the Position component from the entity to read or modify it.
	Position &pos = ecs.getComponent<Position>(entity);
	std::cout << "Position: " << pos.x << ", " << pos.y << std::endl;

	return 0;
}
```

\note For more usage examples, take a look at the [examples](https://github.com/raphaelmayer/easys/blob/master/examples).

## Configuration Options

EasyS provides a couple of overrideable macros to tailor the ECS to your use case:

- \ref EASYS_ENTITY_TYPE - define the integer type used for entity IDs (default: `uint32_t`)
- \ref EASYS_ENTITY_LIMIT - set the maximum number of active entities (default: `10000`) 

### Usage Instructions

Define these macros **before** including EasyS, either in source or via compiler flags.

#### Customizing in Source Code

```
#define EASYS_ENTITY_TYPE uint64_t // Change to 64-bit unsigned integer
#define EASYS_ENTITY_LIMIT 50000   // Increase entity limit
#include <easys/easys.hpp>         // Path to the configuration file
```

#### Customizing with Compiler Flags

**GCC/Clang** on Linux/macOS:
```
g++ -DEASYS_ENTITY_TYPE=uint64_t -DEASYS_ENTITY_LIMIT=50000 -o my_app my_app.cpp
```

**MSVC** on Windows:
```
cl /D EASYS_ENTITY_TYPE=uint64_t /D EASYS_ENTITY_LIMIT=50000 my_app.cpp
```

Depending on your compiler and development environment the exact command might be slightly different.

### Philosophy and User Responsibilities

\note Users are expected to:

- **Register all Components at Compile-Time:** All component types must be specified as template parameters when instantiating the ECS. Accessing a foreign component will result in a compiler error.
- **Check Component Presence Before Access:** Always verify that an entity has a given component before accessing it. Calling `getComponent<T>(entity)` on an entity that lacks `T` will throw a `Easys::KeyNotFoundException`.


