cmake_minimum_required(VERSION 3.20.0)
project(imgui
        VERSION 1.0
        LANGUAGES CXX
        )

set( PUBLIC_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_glfw.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_opengl3.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_internal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imstb_rectpack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imstb_textedit.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imstb_truetype.h"
)

set( SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_opengl3.cpp"
)

add_library("${PROJECT_NAME}" STATIC "${SOURCES}")

target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_20)

target_include_directories("${PROJECT_NAME}"
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC glfw
    PUBLIC Glad
)
