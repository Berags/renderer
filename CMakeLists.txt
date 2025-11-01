# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.22)

# Define the project name, version, and language
project(
        Renderer
        VERSION 1.0
        DESCRIPTION "Parallel Renderer for Shapes"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- OpenMP Configuration ---
find_package(OpenMP REQUIRED)

# --- Dependency: stb_image_write ---
include(FetchContent)
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master # You can pin this to a specific commit hash for stability
)
FetchContent_MakeAvailable(stb)
# This makes the headers available in ${stb_SOURCE_DIR}

# --- Main Executable Target ---
add_executable(
        renderer
        main.cpp
        src/Image.cpp
        src/Renderer.cpp
        src/SequentialRenderer.cpp
        src/ParallelRenderer.cpp
        src/shape/Circle.cpp
        src/shape/Rectangle.cpp
        src/stb_impl.cpp # Special file for STB implementation (see notes)
)

# --- Include Directories ---
target_include_directories(
        renderer
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${stb_SOURCE_DIR} # Add path to the fetched STB headers
)

# --- Link Libraries ---
target_link_libraries(renderer PRIVATE OpenMP::OpenMP_CXX)

# --- Compiler Flags ---
# Set optimization flags.
# -O3 for high optimization
# -march=native to enable all instructions on the host machine (AVX, etc.)
# This is a good way to get auto-vectorization.
target_compile_options(
        renderer
        PRIVATE
        -O3
        -march=native)

# For MSVC (Visual Studio)
if (MSVC)
    target_compile_options(renderer PRIVATE /O2 /arch:AVX2)
    # MSVC uses /openmp instead of -fopenmp
endif ()

set(CMAKE_USE_RELATIVE_PATHS ON)

install(
        TARGETS renderer
        DESTINATION bin)