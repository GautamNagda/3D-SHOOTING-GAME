cmake_minimum_required(VERSION 3.10)
project(CyberStrike3D CXX C)

# Modern C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Weapon.cpp
    src/Collision.cpp
    src/Particle.cpp
    src/Arena.cpp
    src/Camera.cpp
    src/UI.cpp
    src/Core/Window.cpp
    src/Core/Transform.cpp
    src/Renderer/Shader.cpp
    src/Renderer/CubeMesh.cpp
    src/Renderer/Texture.cpp
    external/glad/src/glad.c
)

# Header files for IDE organization
set(HEADERS
    include/Game.h
    include/Player.h
    include/Enemy.h
    include/Weapon.h
    include/Collision.h
    include/Particle.h
    include/Arena.h
    include/Camera.h
    include/UI.h
    include/Core/Window.h
    include/Core/Transform.h
    include/Renderer/Shader.h
    include/Renderer/CubeMesh.h
    include/Renderer/Texture.h
)

# Define executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Linking libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib-mingw/libglfw3dll.a
        opengl32
        gdi32
        user32
        kernel32
    )

    # Automatically copy glfw3.dll to the executable directory post-build
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib-mingw/glfw3.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        GL
        X11
        pthread
        Xrandr
        Xi
        dl
    )
endif()

# Copy shaders and assets to output directory post-build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
