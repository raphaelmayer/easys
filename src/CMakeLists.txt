# Define a static library target named ECSLib with specified source files.
# Add your source files (.cpp) corresponding to the headers here.
add_library(${PROJECT_NAME} STATIC 
    ECSManager.hpp
    VectorRegistry.hpp
    Entity.hpp
    #ECSManager.cpp  # Assuming you have corresponding source files
    #VectorRegistry.cpp
)

# Specify include directories for this target. 
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Explicitly set the linker language if necessary
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)