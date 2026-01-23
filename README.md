
# EasyS: Manage entities and components with minimal fuss

[![Build and test](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml/badge.svg)](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue?style=flat&logo=github)](https://raphaelmayer.github.io/easys/)

EasyS is a minimalist, header-only C++ library designed to streamline the development of applications using the Entity Component System (ECS) architecture. With a focus on simplicity, flexibility, and ease of use, it offers developers an unopinionated foundation to build efficient, high-performance systems without the overhead of external dependencies.
**The library uses a static design approach**, requiring all component types to be registered at compile time through template parameters.

**TLDR:** EasyS provides the essential tools to create, manage, and iterate on entities and components with minimal fuss.


## Quick Start Guide

The Easys::ECS class is the single public interface for EasyS. It provides all functionality for:

    Compile-time component registration
    Entity creation and management
    Component addition, removal, and querying
	Logging

All public operations are available as member functions. See the API Reference for a complete list and descriptions.
This section will walk you through the basics of creating entities, adding components to them, and querying these components.

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

Check out the ```/examples``` directory for more examples on how to use this library or refer to the [documentation](https://raphaelmayer.github.io/easys/).

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
#include <easys/utils.hpp>
#include <easys/view.hpp>
// Add other headers as needed
```
*Keep in mind, that `config.hpp` has to be included before any other EasyS headers.*

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

## Configuration Options

EasyS provides a couple of overrideable macros to tailor the ECS to your use case [documentation](https://raphaelmayer.github.io/easys/d8/dfd/config_8hpp.html):

- \ref EASYS_ENTITY_TYPE  
  Define the integer type used for entity IDs (default: `uint32_t`)

- \ref EASYS_ENTITY_LIMIT  
  Set the maximum number of active entities (default: `10000`)

- \ref EASYS_LOG_ENABLED  
  Master switch for all logging functionality (default: disabled)

- \ref EASYS_LOG_LEVEL  
  Set the global logging severity threshold  
  (`0` = NONE, `1` = ERROR, `2` = INFO, `3` = DEBUG, `4` = TRACE; default: `2`)

- \ref EASYS_LOG_TO_FILE  
  Enable or disable logging to a file (default: disabled)

- \ref EASYS_LOG_FILE_PATH  
  Specify the output file path used when file logging is enabled  
  (default: `"easys_log.txt"`)

- \ref EASYS_LOG_VERBOSITY  
  Enable additional verbose or diagnostic logging  
  (default: disabled)

Define these macros **before** including EasyS, either in source or via compiler flags.

### Customizing in Source Code

```
#define EASYS_ENTITY_TYPE uint64_t // Change to 64-bit unsigned integer
#define EASYS_ENTITY_LIMIT 50000   // Increase entity limit
#include <easys/easys.hpp>         // Path to the configuration file
```

### Customizing with Compiler Flags

**GCC/Clang** on Linux/macOS:
```
g++ -DEASYS_ENTITY_TYPE=uint64_t -DEASYS_ENTITY_LIMIT=50000 -o my_app my_app.cpp
```

**MSVC** on Windows:
```
cl /D EASYS_ENTITY_TYPE=uint64_t /D EASYS_ENTITY_LIMIT=50000 my_app.cpp
```

Depending on your compiler and development environment the exact command might be slightly different.

#### Philosophy and User Responsibilities

Users are expected to:

- **Register all Components at Compile-Time:** All component types must be specified as template parameters when instantiating the ECS. Accessing a foreign component will result in a compiler error.
- **Check Component Presence Before Access:** Always verify that an entity has a given component before accessing it. Calling `getComponent<T>(entity)` on an entity that lacks `T` will throw a `Easys::KeyNotFoundException`.

## Support

If you encounter any issues or have questions regarding the integration process, please feel free to open an issue on the [EasyS GitHub repository](https://github.com/raphaelmayer/EasyS/issues).
