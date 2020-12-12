cmake_minimum_required (VERSION 3.1)
project (OpenRS)

include(cmake/genrev.cmake)

configure_file(
	${CMAKE_SOURCE_DIR}/res/Version.h.in
	${CMAKE_BINARY_DIR}/Version.h
)

set(CMAKE_CXX_STANDARD 14)

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------

add_subdirectory(external)
add_subdirectory(common)
add_subdirectory(cache)
add_subdirectory(openrs)

#--------------------------------------------------------------------
# Create package
#--------------------------------------------------------------------

include(cmake/package.cmake)
