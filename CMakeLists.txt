cmake_minimum_required (VERSION 3.1)
project (OpenRS)

# Generate version information

execute_process(
	COMMAND git rev-parse --abbrev-ref HEAD
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE CMAKE_PROJECT_VERSION_BRANCH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND git log -1 --format=%h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE CMAKE_PROJECT_VERSION_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND git describe --abbrev=0
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE CMAKE_PROJECT_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" CMAKE_PROJECT_VERSION_MAJOR "${CMAKE_PROJECT_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" CMAKE_PROJECT_VERSION_MINOR "${CMAKE_PROJECT_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" CMAKE_PROJECT_VERSION_REVISION "${CMAKE_PROJECT_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" CMAKE_PROJECT_VERSION_BUILD "${CMAKE_PROJECT_VERSION}")
string(TIMESTAMP CMAKE_PROJECT_YEAR "%Y")

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
add_subdirectory(openrs-filesystem)
add_subdirectory(openrs)

