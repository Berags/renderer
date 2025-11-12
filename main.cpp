#include "experiments/Experiments.h"
#include "include/renderer/Renderer.h"

int main() {
  auto renderer = Renderer::Renderer();

  constexpr int kImageWidth = 2048;
  constexpr int kImageHeight = 2048;
  constexpr int kMinShapes = 100;
  constexpr int kMaxShapes = 5000;
  constexpr int kStep = 100;
  constexpr int kNumberOfIterations = 10;

  omp_set_num_threads(8);
#pragma omp parallel
  {
#pragma omp single
    printf("num_threads = %d\n", omp_get_num_threads());
  }

  for (int i = 0; i < kNumberOfIterations; i++) {
    renderer.set_strategy(Renderer::RenderStrategy::SIMPLE_PARALLEL);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_simple_benchmark_results.csv");

    renderer.set_strategy(Renderer::RenderStrategy::OPTIMIZED_PARALLEL);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_optimized_benchmark_results.csv");

    renderer.set_strategy(Renderer::RenderStrategy::SPATIAL_GRID_PARALLEL);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_spatial_grid_benchmark_results.csv");
  }
  return 0;
}
