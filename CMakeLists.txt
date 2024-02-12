# Set the minimum required version of cmake for this project.
cmake_minimum_required(VERSION 3.14)

# Define the project name and version.
project(ECSLib 
    VERSION 1.0 
    DESCRIPTION "An Entity Component System Library"
    LANGUAGES CXX)

# Specify the C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Ensure -std=c++17 not -std=gnu++17

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # support folders in IDEs

include(FetchContent)

# Include directories for the ECS library.
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add subdirectories that will contain the source code for the library.
add_subdirectory(src)

# optional build targets
option(BUILD_EXAMPLES "Build example applications" OFF)
option(BUILD_TESTS "Build tests" ON)

if(BUILD_EXAMPLES)
  #add_subdirectory(examples)
endif()

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

# Installation rules for the library, specify target and destination.
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.hpp")

# Enable warnings if supported by the compiler
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
