
cmake_minimum_required (VERSION 3.14)

find_package (doctest REQUIRED)
find_package (fmt REQUIRED)

add_executable (test-${PROJECT_NAME})

set (app_ test-${PROJECT_NAME})
target_sources (${app_} PRIVATE helper.cpp helper.hpp whirlpool.cpp main.cpp)
target_link_libraries (${app_}
                       PRIVATE ${PROJECT_NAME} whirlpool doctest::doctest fmt::fmt)
target_compile_features (${app_} PRIVATE cxx_std_14)
target_compile_definitions (${app_} PRIVATE DOCTEST_CONFIG_SUPER_FAST_ASSERTS=1)

add_test (NAME ${app_} COMMAND ${app_})
