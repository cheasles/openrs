cmake_minimum_required (VERSION 3.1)
project (OpenRS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(cmake/genrev.cmake)
include(cmake/options.cmake)

configure_file(
	${CMAKE_SOURCE_DIR}/res/Version.h.in
	${CMAKE_BINARY_DIR}/Version.h
)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Compile test modules" ON)

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------

add_subdirectory(external)
add_subdirectory(common)
add_subdirectory(cache)
add_subdirectory(openrs)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)

#--------------------------------------------------------------------
# Create package
#--------------------------------------------------------------------

include(cmake/package.cmake)
