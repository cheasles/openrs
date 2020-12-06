cmake_minimum_required (VERSION 3.1)
set(PROJECT_SUBNAME OpenRS)
project (${CMAKE_PROJECT_NAME}-${PROJECT_SUBNAME} CXX)

# Add source files
file (GLOB_RECURSE SOURCES
	"src/*.cpp"
	"include/${PROJECT_SUBNAME}/*.h"
	"include/${PROJECT_SUBNAME}/*.hpp"
	"${CMAKE_BINARY_DIR}/Version.h"
	)

add_executable(
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
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    frozen
    nlohmann_json::nlohmann_json
    cryptopp-static
    ${CMAKE_PROJECT_NAME}-Common
    ${CMAKE_PROJECT_NAME}-FileSystem)
