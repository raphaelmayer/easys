# Set the minimum required version of cmake for this project.
cmake_minimum_required(VERSION 3.14)

# Define the project name and version.
project(easys 
    VERSION 1.0 
    DESCRIPTION "A Header-only Entity Component System Library"
    LANGUAGES CXX)

# Specify the C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Ensure -std=c++17 not -std=gnu++17

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # support folders in IDEs

# Specify this is an interface library
add_library(${PROJECT_NAME} INTERFACE)

# Specify include directories for consumers of this library
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# optional build targets
option(BUILD_EXAMPLES "Build example applications" OFF)
option(BUILD_TESTS "Build tests" ON)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTS)
  enable_testing() # Enable testing for the project
  add_subdirectory(tests)
endif()

# Installation rules for the library headers
install(DIRECTORY include/easys
        DESTINATION include
)

# Optionally set up installation for the library target itself (for CMake find_package support)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
install(EXPORT ${PROJECT_NAME}Config DESTINATION share/${PROJECT_NAME}/cmake)
