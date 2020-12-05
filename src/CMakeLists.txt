
cmake_minimum_required (VERSION 3.14)

find_package (Python3 COMPONENTS Interpreter)

set (const_table_ ${CMAKE_CURRENT_SOURCE_DIR}/whirlpool.inc)
if (Python3_Interpreter_FOUND)
    add_custom_command (OUTPUT ${const_table_}
                        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen-table.py
                                ${const_table_}
                        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/gen-table.py
                        COMMENT "Generate a Whirlpool constant table")
    add_custom_target (gen-const DEPENDS ${const_table_})
endif ()

target_sources (${PROJECT_NAME} PRIVATE whirlpool.cpp)

target_compile_features (whirlpool PUBLIC cxx_std_14)

add_dependencies (${PROJECT_NAME} gen-const)
