cmake_minimum_required(VERSION 3.10)
project(basketball_game)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(basketball_game main.cpp)

if (EMSCRIPTEN)
    set_target_properties(basketball_game PROPERTIES SUFFIX ".html")
    target_link_options(basketball_game PRIVATE 
        -sUSE_GLFW=3
        -sWASM=1
        -sALLOW_MEMORY_GROWTH=1
        -sLEGACY_GL_EMULATION=1
        -sSTB_IMAGE=1
        --preload-file ${CMAKE_SOURCE_DIR}/pics@/pics
    )
endif() 