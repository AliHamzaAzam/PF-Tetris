cmake_minimum_required(VERSION 3.28)
project(PF_Tetris)

set(CMAKE_CXX_STANDARD 20)

#Add relative path to the resource files
file(COPY GameResources DESTINATION ${CMAKE_BINARY_DIR})

add_executable(PF_Tetris main.cpp)

# Fetch and statically link SFML 2.6 (no system install needed).
# Audio and Network are unused by the game; disabling them avoids dragging in
# their dynamic macOS frameworks (OpenAL/Vorbis/Ogg/FLAC) — only freetype remains.
include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(SFML)

target_compile_definitions(PF_Tetris PRIVATE SFML_STATIC)
target_link_libraries(PF_Tetris PRIVATE sfml-system sfml-window sfml-graphics)
