cmake_minimum_required(VERSION 3.20.0)
project(Elysium
        VERSION 1.0
        LANGUAGES CXX C
        )

add_subdirectory("dependencies/Glad")
add_subdirectory("dependencies/glfw")

add_subdirectory("src/vendor/glm")

set( ${CORE_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Animation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/ImGuiUtility.h"
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

set( ${CORE_SRC}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Animation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/ImGuiUtility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Layer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/LayerStack.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/MathUtility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/OrthographicCameraController.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/ParticleSystem2D.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Random.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Timer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Utility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Window.cpp"
)

set( ${AI_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/AIUtility.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Dense.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/HiddenLayer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Matrix.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Model.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Perceptron.h"
)

set( ${AI_SRC}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/AIUtility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Dense.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/HiddenLayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Matrix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Model.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/AI/Perceptron.cpp"
)

set( ${EVENTS_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Events/ApplicationEvent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Events/Event.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Events/KeyEvent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Events/MouseEvent.h"
)

set( ${NETWORK_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/network_header.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/UDPClient.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/UDPServer.h"
)

set( ${NETWORK_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/network_header.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/UDPClient.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/UDPServer.h"
)

set( ${NETWORK_SRC}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/UDPClient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Network/UDPServer.cpp"
)

set( ${PHYSICS_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Physics/PhysicalBody2D.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Physics/PhysicsSystem2D.h"
)

set( ${PHYSICS_SRC}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Physics/PhysicalBody2D.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Physics/PhysicsSystem2D.cpp"
)

set( ${RENDERER_HEADER}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Assert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Camera.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Framebuffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Indexbuffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Renderer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/RenderUtility.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Shader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Texture.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/VertexArray.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/VertexBuffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/VertexBufferLayout.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/VertexData.h"
)

set( ${RENDERER_SRC}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Framebuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Indexbuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/RenderUtility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Shader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/Texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/VertexArray.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Elysium/Renderer/VertexBuffer.cpp"
)

set( PRIVATE_HEADER
    ${CORE_HEADER}
    ${AI_HEADER}
    ${EVENTS_HEADER}
    ${NETWORK_HEADER}
    ${PHYSICS_HEADER}
    ${RENDERER_HEADER}
)

set( SOURCES
    ${CORE_SRC}
    ${AI_SRC}
    ${EVENTS_SRC}
    ${NETWORK_SRC}
    ${PHYSICS_SRC}
    ${RENDERER_SRC}
)

set( PUBLIC_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/Elysium.h"
)

# Set the compile options for the target
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_compile_options(${PROJECT_NAME} PRIVATE
    # MSVC flags
    $<$<CXX_COMPILER_ID:MSVC>:
        /W3
        /permissive-
    >
    # GCC / Clang flags
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Wall -Wextra -Wpedantic
        -Wnon-virtual-dtor
    >
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        GLFW_INCLUDE_NONE
        _WINSOCK_DEPRECATED_NO_WARNINGS
        _CRT_SECURE_NO_WARNINGS
    >
)

add_library("${PROJECT_NAME}" STATIC "${SOURCES}")

target_include_directories(Elysium
    PRIVATE ${PRIVATE_HEADER}
)

target_link_libraries(Elysium
    PUBLIC Glad
    PUBLIC GLFW
    PUBLIC glm
)