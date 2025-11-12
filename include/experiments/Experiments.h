//
// Created by jacopo on 11/7/25.
//

#ifndef RENDERER_EXPERIMENTS_H
#define RENDERER_EXPERIMENTS_H
#include <omp.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>

#include "renderer/Renderer.h"
#include "utils/Utils.h"

namespace Experiments {
inline void run_benchmark(const Renderer::Renderer &renderer,
                          const int min_shapes, const int max_shapes,
                          const int step, const int image_width,
                          const int image_height,
                          const std::string &output_filename) {
  std::ofstream output_file(output_filename);
  if (!output_file.is_open()) {
    std::cerr << "Error: Could not open output file " << output_filename
              << std::endl;
    return;
  }

  // Write CSV header
  output_file << "Shapes,RenderTimeMs\n";
  std::cout << "Starting benchmark. Results will be saved to "
            << output_filename << std::endl;

  Image image(image_width, image_height,
              "unused.png");  // Create a single image instance for all tests
  std::map<uint32_t, std::vector<std::unique_ptr<Shape::IShape> > > shapes;

  for (uint32_t n = min_shapes; n <= max_shapes; n += step) {
    // check if this number of shapes was already generated
    if (!shapes.contains(n)) {
      shapes[n] = std::vector<std::unique_ptr<Shape::IShape> >();
      Utils::create_shapes(shapes[n], image, n);
    }

    assert(static_cast<size_t>(n) == shapes[n].size());
    // Start the rendering process measuring the time
    const double start_time = omp_get_wtime();
    renderer.render(image, shapes[n]);
    const double end_time = omp_get_wtime();

    const double render_time_ms = (end_time - start_time) * 1000.0;

    std::cout << "Test with " << n << " shapes completed in: " << render_time_ms
              << " ms.\n";
    output_file << n << "," << render_time_ms << "\n";
  }

  output_file.close();
  std::cout << "Benchmark finished.\n";
}
}  // namespace Experiments

#endif  // RENDERER_EXPERIMENTS_H
