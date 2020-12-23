cmake_minimum_required (VERSION 3.1)
set(PROJECT_SUBNAME openrs)
project (${CMAKE_PROJECT_NAME}-${PROJECT_SUBNAME} CXX)

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# mysql
find_package(MySQL REQUIRED)

# sqlite
find_package(SQLite3 REQUIRED)

# Add source files
file (GLOB_RECURSE SOURCES
	"src/*.cpp"
	"include/${PROJECT_SUBNAME}/*.h"
	"include/${PROJECT_SUBNAME}/*.hpp"
	"${CMAKE_BINARY_DIR}/options.h"
	"${CMAKE_BINARY_DIR}/Version.h"
	)

add_executable(
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
    Threads::Threads
    frozen
    nlohmann_json::nlohmann_json
    ${MySQL_LIBRARIES}
    ${SQLite3_LIBRARIES}
    cryptopp-static
    ${CMAKE_PROJECT_NAME}-common
    ${CMAKE_PROJECT_NAME}-cache
    ${CMAKE_PROJECT_NAME}-database
    ${CMAKE_PROJECT_NAME}-game)
