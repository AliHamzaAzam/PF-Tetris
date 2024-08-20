cmake_minimum_required(VERSION 3.28)
project(PF_Tetris)

set(CMAKE_CXX_STANDARD 20)

#Add relative path to the resource files
file(COPY GameResources DESTINATION ${CMAKE_BINARY_DIR})

add_executable(PF_Tetris main.cpp)

if (WIN32)
    include_directories(C:/SFML-2.5.1/include)
else ()
    include_directories(/usr/local/include)
endif ()

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})

target_link_libraries(PF_Tetris sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
