#include <ctime>
#include <iostream>

#include "include/Image.h"
#include "include/renderer/Renderer.h"
#include "include/shape/Circle.h"
#include <omp.h>

#include "experiments/Experiments.h"
#include "utils/Utils.h"

int main() {
    auto renderer = Renderer::Renderer();
    renderer.setStrategy(Renderer::RenderStrategy::SPATIAL_GRID_PARALLEL);

    constexpr int imageWidth = 2048;
    constexpr int imageHeight = 2048;

    renderer.setStrategy(Renderer::RenderStrategy::SEQUENTIAL);
    Experiments::runBenchmark(renderer, 100, 5000, 10, imageWidth, imageHeight, "sequential_benchmark_results.csv");

    renderer.setStrategy(Renderer::RenderStrategy::SIMPLE_PARALLEL);
    Experiments::runBenchmark(renderer, 100, 5000, 10, imageWidth, imageHeight, "simple_benchmark_results.csv");

    renderer.setStrategy(Renderer::RenderStrategy::OPTIMIZED_PARALLEL);
    Experiments::runBenchmark(renderer, 100, 5000, 10, imageWidth, imageHeight, "optimized_benchmark_results.csv");

    renderer.setStrategy(Renderer::RenderStrategy::SPATIAL_GRID_PARALLEL);
    Experiments::runBenchmark(renderer, 100, 5000, 10, imageWidth, imageHeight, "spatial_grid_benchmark_results.csv");

    return 0;
}
