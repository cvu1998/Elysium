cmake_minimum_required(VERSION 3.13.0)
project(Elysium
        VERSION 1.0
        LANGUAGES CXX
        )

set( PUBLIC_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Animation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Animation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Input.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/KeyCodes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Layer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/LayerStack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Math.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/MathUtility.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/MouseCodes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/OrthographicCameraController.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/ParticleSystem2D.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Random.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Timer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Timestep.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Utility.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Window.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium.h"
)

set( SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Animation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Animation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Input.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/KeyCodes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Layer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/LayerStack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Math.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/MathUtility.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/MouseCodes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/OrthographicCameraController.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/ParticleSystem2D.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Random.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Timer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Timestep.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Utility.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Window.h"
)

# Set the compile options for the target
target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_20)

add_library("${PROJECT_NAME}" STATIC "${SOURCES}")

target_link_libraries(Elysium
                    PUBLIC Glad
                    PUBLIC GLFW
                    )

target_include_directories(Glad
    PUBLIC  include/
    PRIVATE src/
)

# Generate the export headers for the project.
include(GenerateExportHeader)
generate_export_header("${PROJECT_NAME}" 
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/exports/mymath_export.h"
)

set( PUBLIC_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/exports/mymath_export.h"
)

# If we are building a static library, we need to add a prefix on Windows
# Since both the shared and static version generate a .lib file.
if( WIN32 )
    set( LIBRARY_PREFIX "static_")
    set_target_properties("${PROJECT_NAME}" PROPERTIES PREFIX "static_")
endif()

# Set the public headers and debug postfix
set_target_properties("${PROJECT_NAME}" PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
    DEBUG_POSTFIX "d"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# Add the include directories to the builds and the installs
target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}/${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/exports>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Generate the export components for the package.
install(TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}-export"
        COMPONENT Devel EXCLUDE_FROM_ALL
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
        RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
        PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}"
)

# Install the targets file at the library path so we can easily include
# this library to other projects.
install(EXPORT "${PROJECT_NAME}-export"
    FILE "${PROJECT_NAME}Targets.cmake"
    DESTINATION "${ConfigPackageLocation}"
    COMPONENT Devel EXCLUDE_FROM_ALL
)

# Install the PDB files if they were generated.
if( MSVC AND BUILD_SHARED_LIBS )
    install(
        FILES $<TARGET_PDB_FILE:${PROJECT_NAME}>
        DESTINATION "${RUNTIME_INSTALL_DIR}"
        COMPONENT Devel EXCLUDE_FROM_ALL
        OPTIONAL
    )
endif()