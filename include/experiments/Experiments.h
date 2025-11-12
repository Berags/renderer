//
// Created by jacopo on 11/7/25.
//

#ifndef RENDERER_EXPERIMENTS_H
#define RENDERER_EXPERIMENTS_H
#include <omp.h>

#include <cassert>
#include <fstream>
#include <map>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "renderer/Renderer.h"
#include "utils/Utils.h"

namespace Experiments {
inline void run_benchmark(const Renderer::Renderer &renderer,
                          const int min_shapes, const int max_shapes,
                          const int step, const int image_width,
                          const int image_height,
                          const std::string &output_filename) {
  std::string csv_content;
  FILE *output_file = fopen(output_filename.c_str(), "w");
  if (output_file == nullptr) {
    LOG(ERROR) << "Error: Could not open output file " << output_filename;
    return;
  }

  // write csv header
  absl::StrAppend(&csv_content, "Shapes,RenderTimeMs\n");

  LOG(INFO) << "Running benchmark from " << min_shapes << " to " << max_shapes
            << " shapes, step " << step << ", image size " << image_width << "x"
            << image_height << ".";

  Image image = *Image::Create(
      image_width, image_height,
      "unused.png");  // Create a single image instance for all tests
  std::map<uint32_t, std::vector<std::unique_ptr<Shape::IShape> > > shapes;

  for (uint32_t n = min_shapes; n <= max_shapes; n += step) {
    // check if this number of shapes was already generated
    if (!shapes.contains(n)) {
      shapes[n] = std::vector<std::unique_ptr<Shape::IShape> >();
      Utils::create_shapes(shapes[n], image, n);
    }

    CHECK(static_cast<size_t>(n) == shapes[n].size())
        << "Shape generation error. Expected " << n << " shapes, got "
        << shapes[n].size();
    // Start the rendering process measuring the time
    const double start_time = omp_get_wtime();
    renderer.render(image, shapes[n]);
    const double end_time = omp_get_wtime();

    const double render_time_ms = (end_time - start_time) * 1000.0;

    std::string output =
        absl::StrFormat("Rendered %d shapes in %.2f ms.", n, render_time_ms);
    printf("%s\n", output.c_str());
    absl::StrAppendFormat(&csv_content, "%d,%.6f\n", n, render_time_ms);
  }
  const int result = fprintf(output_file, "%s", csv_content.c_str());
  fclose(output_file);
  if (!result) {
    LOG(ERROR) << "Error: Could not write output file " << output_filename;
    return;
  }

  LOG(INFO) << "Done. Results saved to " << output_filename;
}
}  // namespace Experiments

#endif  // RENDERER_EXPERIMENTS_H
