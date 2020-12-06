cmake_minimum_required (VERSION 3.1)
set(PROJECT_SUBNAME cache)
project (${CMAKE_PROJECT_NAME}-${PROJECT_SUBNAME} CXX)

# Add source files
file (GLOB_RECURSE SOURCES
	"src/*.cpp"
	"include/${CMAKE_PROJECT_NAME}/${PROJECT_SUBNAME}/*.h"
	"include/${CMAKE_PROJECT_NAME}/${PROJECT_SUBNAME}/*.hpp"
	"${CMAKE_BINARY_DIR}/Version.h"
	)

add_library(
    ${PROJECT_NAME}
    ${SOURCES}
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

# pthread
find_package(LibLZMA)
target_link_libraries(${PROJECT_NAME}
    ${LIBLZMA_LIBRARIES}
    ${CMAKE_PROJECT_NAME}-Common
    cryptopp-static)

