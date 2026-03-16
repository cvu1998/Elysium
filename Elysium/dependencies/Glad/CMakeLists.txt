cmake_minimum_required(VERSION 3.13.0)
project(Glad
        VERSION 1.0
        LANGUAGES CXX
        )

set( PUBLIC_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h"
)

set( SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c"
)

# Set the compile options for the target
target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_20)

add_library("${PROJECT_NAME}" STATIC "${SOURCES}")

target_include_directories(Glad
    PUBLIC  include/
    PRIVATE src/
)