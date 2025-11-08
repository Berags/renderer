#include "include/renderer/Renderer.h"

#include "experiments/Experiments.h"

int main() {
    auto renderer = Renderer::Renderer();

    constexpr int imageWidth = 2048;
    constexpr int imageHeight = 2048;
    constexpr int minShapes = 100;
    constexpr int maxShapes = 2000;
    constexpr int step = 100;
    constexpr int numberOfIterations = 10;

    for (int i = 0; i < numberOfIterations; i++) {
        renderer.setStrategy(Renderer::RenderStrategy::SEQUENTIAL);
        Experiments::runBenchmark(renderer, minShapes, maxShapes, step, imageWidth, imageHeight,
                                  "results/" + std::to_string(i) + "_sequential_benchmark_results.csv");

        renderer.setStrategy(Renderer::RenderStrategy::SIMPLE_PARALLEL);
        Experiments::runBenchmark(renderer, minShapes, maxShapes, step, imageWidth, imageHeight,
                                  "results/" + std::to_string(i) + "_simple_benchmark_results.csv");

        renderer.setStrategy(Renderer::RenderStrategy::OPTIMIZED_PARALLEL);
        Experiments::runBenchmark(renderer, minShapes, maxShapes, step, imageWidth, imageHeight,
                                  "results/" + std::to_string(i) + "_optimized_benchmark_results.csv");

        renderer.setStrategy(Renderer::RenderStrategy::SPATIAL_GRID_PARALLEL);
        Experiments::runBenchmark(renderer, minShapes, maxShapes, step, imageWidth, imageHeight,
                                  "results/" + std::to_string(i) + "_spatial_grid_benchmark_results.csv");
    }
    return 0;
}
