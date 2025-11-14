# Parallel Shape Renderer with OpenMP

A high-performance C++ renderer demonstrating parallel programming techniques with OpenMP for 2D shape rendering. This project implements several rendering strategies, from a simple sequential loop to an advanced, highly-optimized spatial grid, to compare their performance.

The primary challenge is to correctly and efficiently render thousands of (semi)transparent shapes, which requires proper Z-axis sorting for alpha blending.

## Project Description

This renderer is designed to draw circles and rectangles onto a 2D image. Each shape has a Z-coordinate (depth), and the final color of each pixel is determined by alpha-blending all shapes that cover its center, in order from back-to-front.

The project explores different optimization strategies:

1.  **Sequential:** A single-threaded, brute-force baseline.
2.  **Simple Parallel:** A tile-based approach using OpenMP's `parallel for`.
3.  **Optimized Parallel:** Adds data-oriented design (AoS) and SIMD vectorization.
4.  **Spatial Grid Parallel:** A highly-optimized, two-pass algorithm using a spatial hash grid, fine-grained locking, and SIMD (SoA) transformations for maximum performance.

The `main.cpp` entry point is configured to run a benchmark comparing all four strategies, saving the results to CSV files.

## Features

  * **Shapes:** Renders Circles and Rectangles.
  * **Alpha Blending:** Correctly handles semi-transparent shapes by sorting by Z-index and blending back-to-front.
  * **Hit Detection:** A pixel is drawn if its center (x+0.5, y+0.5) is inside a shape.
  * **Parallelism:** Uses OpenMP for multi-core processing, including `parallel for`, `collapse`, `simd`, and `omp_lock_t`.
  * **Strategy Pattern:** Easily switch between rendering algorithms using a common `Renderer` interface.
  * **Output:** Saves renderings as PNG files using `stb_image_write`.

## Requirements

  * **CMake 3.22+**
  * **C++23 compatible compiler** (e.g., GCC 13+, Clang 17+)
  * **OpenMP Support**
  * **Git** (for fetching dependencies)
  * **Internet Connection** (for `FetchContent` on first build)

This project fetches its dependencies (`absl` and `stb`) automatically using `FetchContent`.

## Building and Running

### Build

We use CMake for building. This project is optimized for release builds with native compiler flags enabled for best performance.

```bash
# 1. Clone the repository
git clone https://github.com/Berags/renderer.git
cd renderer

# 2. Create a build directory
mkdir build
cd build

# 3. Configure CMake (use -DCMAKE_BUILD_TYPE=Release for performance)
# This will also fetch Abseil and stb_image
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. Build the project
# (CMakeLists.txt sets parallel level to 8, you can override this)
cmake --build . -j $(nproc) 
```

### Run

The `main` executable is a benchmark harness that runs all implemented strategies.

```bash
# Run the benchmark from the build directory
./renderer
```

The program will:

1.  Run `kNumberOfIterations` (e.g., 10) benchmarking loops.
2.  In each loop, it renders scenes from `kMinShapes` to `kMaxShapes` (e.g., 100 to 5000).
3.  It times each of the four rendering strategies (`kSequential`, `kSimpleParallel`, `kOptimizedParallel`, `kSpatialGridParallel`).
4.  It saves the timing results (in milliseconds) to CSV files in a `results/` directory (e.g., `results/0_sequential_benchmark_results.csv`).

## Solution Specification: Renderer Strategies

This project implements four distinct rendering strategies to demonstrate a progressive approach to parallel optimization.

### 1\. `SequentialRenderer` (Baseline)

This is a straightforward, single-threaded implementation. It serves as the performance baseline.

  * **Algorithm:**
    1.  Sorts the *entire* list of N shapes by their Z-index.
    2.  Iterates over every pixel `(y, x)` in the image.
    3.  For each pixel, it iterates through *all N sorted shapes*.
    4.  If the pixel's center is inside the shape, it blends that shape's color.
  * **Complexity:** `O(ImageWidth * ImageHeight * NumShapes)`
  * **Bottleneck:** Extremely high computational complexity. Inefficient cache use.

### 2\. `SimpleParallelRenderer` (Tiled Parallelism)

This is the first parallel attempt, which introduces **data parallelism** by dividing the image into tiles.

  * **Algorithm:**
    1.  Sorts the *entire* list of N shapes by Z-index (same as sequential).
    2.  Divides the image into 32x32 tiles.
    3.  Uses `#pragma omp parallel for collapse(2)` to assign tiles to different OpenMP threads. 
    4.  Within each tile, the algorithm is the same as the sequential one: iterate every pixel and check it against *all N shapes*.
  * **Optimization:** Achieves good parallel speedup by dividing pixel work across cores. Improves cache locality slightly, as threads work on a 32x32 pixel area.
  * **Bottleneck:** The core computation is still `O(NumPixels * NumShapes)`. Every thread still needs to read the entire list of shapes, leading to high memory bandwidth usage and cache contention.

### 3\. `OptimizedParallelRenderer` (AoS + SIMD)

This version introduces **data-oriented design** (AoS) and **vectorization** (SIMD).

  * **Algorithm:**
    1.  **Data Transformation (AoS):** Converts the `std::vector<std::unique_ptr<Shape::IShape>>` into a `std::vector<RenderItem>`. `RenderItem` is a flat `struct` (Array of Structures) which improves data locality by keeping related shape data contiguous in memory.
    2.  Sorts the `std::vector<RenderItem>` by Z-index.
    3.  Uses the same `#pragma omp parallel for collapse(2)` tiling strategy as the simple version.
    4.  **Vectorization:** Inside the tile's pixel loops, `#pragma omp simd` is applied to the inner `x` loop. This instructs the compiler to use SIMD instructions (like AVX) to process multiple pixels at once, performing the containment checks and blending in parallel.
  * **Optimization:** `RenderItem` improves cache-friendliness. `#pragma omp simd` provides a significant speedup on the inner-most loop.
  * **Bottleneck:** Still `O(NumPixels * NumShapes)`. The main performance killer—checking every shape for every pixel—remains.

### 4\. `SpatialGridParallelRenderer` (Advanced)

This is the most complex and efficient solution. It completely changes the algorithm to a **two-pass spatial-hashing** approach to solve the core bottleneck.

  * **Algorithm (Pass 1: Build Spatial Index):**

    1.  A "spatial index" (`std::vector<std::vector<const RenderItem*>>`) is created, where each entry corresponds to one 32x32 tile.
    2.  A `std::vector<RenderItem>` is created first (similar to `OptimizedParallelRenderer`).
    3.  A `#pragma omp parallel for` loop iterates over *all N shapes*.
    4.  For each shape, it calculates its bounding box.
    5.  It identifies all tiles that the bounding box overlaps.
    6.  **Synchronization:** The thread adds a pointer to the shape's `RenderItem` to the `spatial_index` list for *each* tile it overlaps. Since multiple threads can write to the same tile's list simultaneously, this is a **data race**. The race is prevented by using a `std::vector<omp_lock_t>`, with one lock per tile. A thread must `omp_set_lock` for a specific tile before adding to its list and `omp_unset_lock` after.

  * **Algorithm (Pass 2: Render Tiles):**

    1.  Uses `#pragma omp parallel for collapse(2)` to iterate over all tiles.
    2.  **Key Optimization:** Each thread retrieves the *local* list of shapes for its tile (`local_shapes = spatial_index[tile_index]`). Instead of N (all shapes), it now only processes `k` shapes (where `k` is the number of shapes overlapping this tile, `k << N`).
    3.  Sorts this *local* list `k` by Z-index (`std::ranges::stable_sort`).
    4.  **Data Transformation (SoA):** Converts the `local_shapes` (AoS) into a `TileRenderDataSoA` (Structure of Arrays). This layout (e.g., all circle X-coords in one `vector`, all Y-coords in another) is ideal for SIMD operations.
    5.  **Vectorization:** For each pixel in the tile, two `#pragma omp simd` loops iterate over the SoA data, performing containment tests for all `k` circles and rectangles and saving results to boolean masks.
    6.  A `merge_colours` step efficiently combines the "hit" shapes from the circle and rectangle masks back into a single, Z-sorted list.
    7.  The final pixel color is blended, and `image.set_pixel` is called.
