# EasyS: Manage entities and components with minimal fuss

[![Build and test](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml/badge.svg)](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml)

EasyS is a minimalist, header-only C++ library designed to streamline the development of applications using the Entity Component System (ECS) architecture. With a focus on simplicity, flexibility, and ease of use, it offers developers an unopinionated foundation to build efficient, high-performance systems without the overhead of external dependencies.

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
	// Create an instance of the ECS class to manage our entities and components.
	ECS ecs;

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

Easys requires a compiler that supports at least C++20. This is mainly because of the use of concepts and might change in the future.

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
include(FetchContent)
FetchContent_Declare(
  easys
  GIT_REPOSITORY https://github.com/raphaelmayer/easys.git
  GIT_TAG master # or any version tag you prefer
)
FetchContent_MakeAvailable(easys)
target_link_libraries(your_target_name PRIVATE easys)
```

### Package managers

***Coming soon***

## Configuration Options

Easys provides a couple of configuration options to tailor the ECS to your use case:

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
