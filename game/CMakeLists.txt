cmake_minimum_required (VERSION 3.1)
set(PROJECT_SUBNAME game)
project (${CMAKE_PROJECT_NAME}-${PROJECT_SUBNAME} CXX)

# Add source files
file (GLOB_RECURSE SOURCES
	"src/*.cpp"
	"include/${PROJECT_SUBNAME}/*.h"
	"include/${PROJECT_SUBNAME}/*.hpp"
	"${CMAKE_BINARY_DIR}/options.h"
	"${CMAKE_BINARY_DIR}/Version.h"
	)

add_library(
    ${PROJECT_NAME}
    ${SOURCES}
    )

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_BINARY_DIR}
    )

target_include_directories (
    ${PROJECT_NAME}
    PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_include_directories (
    ${PROJECT_NAME}
    INTERFACE
    include
    )

target_link_libraries(${PROJECT_NAME}
    cryptopp-static
    frozen
    ${CMAKE_PROJECT_NAME}-common
    ${CMAKE_PROJECT_NAME}-database)
