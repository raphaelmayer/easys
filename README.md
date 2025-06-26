
# EasyS: Manage entities and components with minimal fuss

[![Build and test](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml/badge.svg)](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue?style=flat&logo=github)](https://raphaelmayer.github.io/easys/)

EasyS is a minimalist, header-only C++ library designed to streamline the development of applications using the Entity Component System (ECS) architecture. With a focus on simplicity, flexibility, and ease of use, it offers developers an unopinionated foundation to build efficient, high-performance systems without the overhead of external dependencies.

**The library now uses a static design approach**, requiring all component types to be registered at compile time through template parameters. This shift from runtime to compile-time registration improves performance, enables stronger type safety, and eliminates runtime type checks, while still maintaining a clean and lightweight API.

**TLDR:** EasyS provides the essential tools to create, manage, and iterate on entities and components with minimal fuss.


## Quick Start Guide

This section will walk you through the basics of creating entities, adding components to them, and querying these components.

```
#include <easys/ecs.hpp>
#include <easys/entity.hpp>
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

Check out the ```/examples``` directory for more examples on how to use this library.

## Integration Guide

### Requirements

EasyS requires a compiler that supports at least C++20. This is because of the use of concepts and templated lambda functions and is subject to change. 

### Direct inclusion

Since EasyS is a header-only library, you can just include the whole library in your project and use it directly.

1. **Copy or Clone EasyS**: Place the contents of the  `/include` directory into your project's directory structure.

2. **Update Include Paths**: Ensure your compiler includes the path to EasyS. 

3. **Include the Headers**: In your source files, directly include the header file from EasyS: 
```cpp
#include <easys/easys.hpp>
```

### Header Inclusion Options

#### Single-Header Convenience
For simplicity and quick integration, we offer a single-header file that includes all the functionalities of our library.
Include `easys.hpp` for full library access:
```cpp
#include <easys/easys.hpp>
```

#### Individual Headers for Specific Needs

You can also include individual headers, if you do not need all components:
```cpp
#include <easys/config.hpp>
#include <easys/entity.hpp>
#include <easys/ecs.hpp>
// Add other headers as needed
```
Keep in mind, that `config.hpp` has to be included before any other EasyS headers.

### Using CMake

EasyS supports CMake integration, allowing you to easily add it to your CMake-based projects.

#### As a Subdirectory

If you've cloned or downloaded EasyS into your project's directory structure, you can add it as a subdirectory in your `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/easys)
target_link_libraries(your_target_name INTERFACE easys)
```

Replace `path/to/easys` with the actual path to EasyS within your project, and `your_target_name` with the name of your project's target.

#### Using FetchContent (CMake 3.11+)

CMake's FetchContent module allows you to dynamically download and include libraries directly in your build process. To integrate EasyS using FetchContent:

```cmake
FetchContent_Declare(
    EASYS
    GIT_REPOSITORY  https://github.com/raphaelmayer/easys
    GIT_TAG         master # or any version you prefer
    GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(EASYS)
target_link_libraries(your_target_name PRIVATE easys)
```

### Package managers

***Coming soon***

### ECS Library Documentation

All types and functions are defined within the `Easys` namespace. This section documents the core interface of the EasyS ECS library, including entity management and component operations..

#### Constructor

- **`ECS()`**
  - Initializes the ECS with a predefined maximum number of entities (`MAX_ENTITIES`). All entity IDs are initially available for assignment.
- **`ECS(const std::set<Entity>& entities)`**
  - Initializes the ECS with a specific set of entities. This constructor is useful for creating a new ECS instance based on a subset of entities from another instance or a predefined list.

#### Entity Management

- **`Entity addEntity()`**
  - Adds a new entity to the ECS and return the entity id. If the maximum number of entities (`MAX_ENTITIES`) is reached, throws a `std::runtime_error`.
- **`void removeEntity(const Entity e)`**
  - Removes an entity and all its associated components from the ECS, making its ID available for reuse.
- **`bool hasEntity(const Entity e) const`**
  - Checks if an entity exists within the ECS.
- **`const std::set<Entity>& getEntities() const`**
  - Returns a reference to the set of all entities.
- **`const std::vector<Entity>& getEntitiesByComponent<T>() const`**
  - Returns a vector of entities that have a component of type `T`.
- **`std::vector<Entity> getEntitiesByComponents<Ts...>() const`**
  - Returns a vector of entities that have all of the specified component types `Ts...`.
- **`size_t getEntityCount() const`**
  - Returns the total number of entities in the ECS.

#### Component Management

- **`void addComponent<T>(const Entity e, const T c)`**
  - Adds a component of type `T` to an entity `e`. If the entity is already associated with a component `T`, update it.
- **`void removeComponent<T>(const Entity e)`**
  - Removes a component of type `T` from an entity `e`.
- **`void removeComponents<Ts...>(const Entity e)`**
	- Removes components of types `Ts...` from an entity `e`. If the template parameters are omitted, it removes all components from the entity.
- **`T& getComponent<T>(const Entity e)`**
  - Retrieves a reference to a component of type `T` from an entity `e`. 
- **`bool hasComponent<T>(const Entity e) const`**
  - Checks if an entity `e` has a component of type `T`.
- **`size_t getComponentCount<Ts...>() const`**
  - Returns the total count of components of types `Ts...` within the ECS. If template parameters are omitted, it returns the total count of all component types.
 
#### Clearing Methods

- **`void clearComponents<Ts...>()`**
  - Removes components of types `Ts...` from all entities within the ECS. If template parameters are omitted only all types of components are cleared.
- **`void clear()`**
  - Clears all entities and components from the ECS, resetting it to its initial state.

#### Philosophy and User Responsibilities

Users are expected to:

- **Register all Components at Compile-Time:** All component types must be specified as template parameters when instantiating the ECS. Accessing a foreign component will result in a compiler error.
- **Check Component Presence Before Access:** Always verify that an entity has a given component before accessing it. Calling `getComponent<T>(entity)` on an entity that lacks `T` will throw a `Easys::KeyNotFoundException`.

## Configuration Options

EasyS provides a couple of configuration options to tailor the ECS to your use case:

- **```EASYS_ENTITY_TYPE```:** Defines the data type of the entity identifier. Users can choose any *unsigned integer type*. 
By default, it is set to ```uint32_t```. 

- **```EASYS_ENTITY_LIMIT```:** Specifies the maximum number of entities that can be created. 
This limit is set to ```10000``` by default. 

### Usage Instructions

To use these, define the corresponding macros before including the main file in your project. This can be done either directly in the source code or through compiler flags.

#### Customizing in Source Code

Before including this configuration file, define the macros as shown below:

```
#define EASYS_ENTITY_TYPE uint64_t // Change to 64-bit unsigned integer
#define EASYS_ENTITY_LIMIT 50000   // Increase entity limit
#include <easys/config.hpp>	   // Path to the configuration file
```

#### Customizing with Compiler Flags

Alternatively, you can use compiler flags to define these macros, avoiding modifications to the source code, for example:

```g++ -DEASYS_ENTITY_TYPE=uint64_t -DEASYS_ENTITY_LIMIT=50000 -o my_application my_application.cpp```

Replace g++ with the appropriate compiler command for your development environment if different.

## Support

If you encounter any issues or have questions regarding the integration process, please feel free to open an issue on the [EasyS GitHub repository](https://github.com/raphaelmayer/EasyS/issues).
