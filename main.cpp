#include <omp.h>

#include "absl/log/check.h"
#include "absl/log/initialize.h"
#include "absl/strings/str_format.h"
#include "experiments/Experiments.h"
#include "include/renderer/Renderer.h"

int main() {
  absl::InitializeLog();
  auto renderer = Renderer::Renderer();

  constexpr int kNumberOfIterations = 10;

  printf("System's total number of threads: %d\n", omp_get_max_threads());

  for (int i = 0; i < kNumberOfIterations; i++) {
    constexpr int kStep = 100;
    constexpr int kMaxShapes = 5000;
    constexpr int kMinShapes = 100;
    constexpr int kImageHeight = 2048;
    constexpr int kImageWidth = 2048;

    std::string console_output = absl::StrFormat("Starting run n. %d", i);
    printf("%s\n", console_output.c_str());

    renderer.set_strategy(Renderer::RenderStrategy::kSequential);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_sequential_benchmark_results.csv");

    renderer.set_strategy(Renderer::RenderStrategy::kSimpleParallel);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_simple_benchmark_results.csv");

    renderer.set_strategy(Renderer::RenderStrategy::kOptimizedParallel);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_optimized_benchmark_results.csv");

    renderer.set_strategy(Renderer::RenderStrategy::kSpatialGridParallel);
    Experiments::run_benchmark(
        renderer, kMinShapes, kMaxShapes, kStep, kImageWidth, kImageHeight,
        "results/" + std::to_string(i) + "_spatial_grid_benchmark_results.csv");

    console_output = absl::StrFormat("Run n. %d done!", i);
    printf("%s\n", console_output.c_str());
  }
  return 0;
}
