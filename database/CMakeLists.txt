cmake_minimum_required (VERSION 3.1)
set(PROJECT_SUBNAME database)
project (${CMAKE_PROJECT_NAME}-${PROJECT_SUBNAME} CXX)

# mysql
find_package(MySQL REQUIRED)

# sqlite
find_package(SQLite3 REQUIRED)

# Add source files
file (GLOB_RECURSE SOURCES
	"src/*.cpp"
	"include/${PROJECT_SUBNAME}/*.h"
	"include/${PROJECT_SUBNAME}/*.hpp"
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
    ${MySQL_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
    ${QTL_INCLUDE_DIR}
    )

target_include_directories (
    ${PROJECT_NAME}
    INTERFACE
    include
    )

target_link_libraries(${PROJECT_NAME}
    ${MySQL_LIBRARIES}
    ${SQLite3_LIBRARIES}
    ${CMAKE_PROJECT_NAME}-common)
