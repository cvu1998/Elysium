cmake_minimum_required(VERSION 3.13.0)
project(Elysium VERSION 1.0 LANGUAGES CXX)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build GLFW library only")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build GLFW library only")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build GLFW library only")

add_subdirectory(Dependencies/glfw)
add_subdirectory(Dependencies/Glad)

set(CMAKE_CXX_FLAGS "-std=c++17")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

#add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE Glad)
message("Compilation flags for ${PROJECT_NAME}: ${CMAKE_CXX_FLAGS}")