cmake_minimum_required(VERSION 3.24)

project(BomberMan
    VERSION 0.1.0
    DESCRIPTION "SDL3 BomberMan"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Offline-friendly SDL3 discovery:
# 1. Put an SDL3 development package under external/SDL3, or
# 2. Pass -DSDL3_ROOT=C:/path/to/SDL3, or
# 3. Install SDL3 so find_package(SDL3 CONFIG) can locate it.
set(SDL3_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3" CACHE PATH "Path to an SDL3 development package")

foreach(_sdl3_prefix IN LISTS SDL3_ROOT)
    if(EXISTS "${_sdl3_prefix}")
        list(APPEND CMAKE_PREFIX_PATH "${_sdl3_prefix}")
        list(APPEND CMAKE_PREFIX_PATH "${_sdl3_prefix}/cmake")
        list(APPEND CMAKE_PREFIX_PATH "${_sdl3_prefix}/lib/cmake/SDL3")
    endif()
endforeach()

find_package(SDL3 CONFIG REQUIRED)

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

add_executable(sdl3_hello
    src/main.cpp
    src/Math.cpp
    src/Game.cpp
    src/Resource.cpp
    src/InputManager.cpp
    src/Player.cpp
    src/Scene.cpp
    src/GrenadeTestScene.cpp
    src/SceneManager.cpp
    src/DebugDraw.cpp
    src/SpriteAnimation.cpp
    src/Entity.cpp
    src/Block.cpp
    src/BreakableBlock.cpp
    src/Collectible.cpp
    src/Door.cpp
    src/Direction.cpp
    src/DebugUi.cpp
    src/Grenade.cpp
    src/Bomb.cpp
    src/BombTestScene.cpp
)

target_link_libraries(sdl3_hello PRIVATE SDL3::SDL3 imgui)

target_include_directories(sdl3_hello PRIVATE
    external/glm
)

target_compile_definitions(sdl3_hello PRIVATE
    SDL_MAIN_USE_CALLBACKS=0
)

if(MSVC)
    target_compile_options(sdl3_hello PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdl3_hello PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy runtime DLLs next to the executable on Windows when SDL3 provides them as
# imported target runtime dependencies.
if(WIN32)
    add_custom_command(TARGET sdl3_hello POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:sdl3_hello>" $<TARGET_RUNTIME_DLLS:sdl3_hello>
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endif()
