# examples

add_executable(basic "basic.cpp")
target_link_libraries(basic PRIVATE ${PROJECT_NAME})
add_test(NAME basic COMMAND basic)