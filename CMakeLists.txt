cmake_minimum_required(VERSION 4.1)
project(renderer)

set(CMAKE_CXX_STANDARD 26)

add_executable(renderer main.cpp
        src/Renderer.cpp
        include/Renderer.h
        src/SequentialRenderer.cpp
        include/SequentialRenderer.h
        src/ParallelRenderer.cpp
        include/ParallelRenderer.h)
