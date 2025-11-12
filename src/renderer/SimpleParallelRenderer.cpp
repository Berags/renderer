//
// Created by jacopo on 10/28/25.
//

#include "../../include/renderer/SimpleParallelRenderer.h"

#include <algorithm>

void Renderer::SimpleParallelRenderer::render(
    Image &image,
    const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
  // Create a vector of raw pointers to shapes to allow sorting without moving
  // ownership.
  std::vector<const Shape::IShape *> sorted_shapes;
  sorted_shapes.reserve(shapes.size());
  for (const auto &ptr : shapes) {
    sorted_shapes.push_back(ptr.get());
  }

  // Sort shapes by Z-index (ascending) to ensure correct alpha blending from
  // back to front.
  std::ranges::stable_sort(sorted_shapes, std::ranges::less{},
                           &Shape::IShape::get_z);

  const auto width = image.get_width();
  const auto height = image.get_height();

  // Define the size of the tiles for tile-based rendering.
  constexpr uint16_t kTileSize = 32;
  // Calculate the number of tiles needed to cover the image area.
  const uint16_t num_tiles_x = (width + kTileSize - 1) / kTileSize;
  const uint16_t num_tiles_y = (height + kTileSize - 1) / kTileSize;

  // Parallelize the rendering process over the tiles using OpenMP.
  // The `collapse(2)` clause flattens the nested loops over tiles into a single
  // parallel loop.
  #pragma omp parallel for collapse(2)
  for (uint16_t ty = 0; ty < num_tiles_y; ++ty) {
    for (uint16_t tx = 0; tx < num_tiles_x; ++tx) {
      // Determine the pixel boundaries for the current tile.
      const uint16_t y_start = ty * kTileSize;
      const uint16_t x_start = tx * kTileSize;
      const uint16_t y_end =
          std::min(static_cast<uint16_t>(y_start + kTileSize), height);
      const uint16_t x_end =
          std::min(static_cast<uint16_t>(x_start + kTileSize), width);

      // Iterate over each pixel within the current tile.
      for (uint16_t y = y_start; y < y_end; ++y) {
        for (uint16_t x = x_start; x < x_end; ++x) {
          // Calculate the pixel's center coordinates.
          const float px = static_cast<float>(x) + 0.5f;
          const float py = static_cast<float>(y) + 0.5f;

          // Initialize with a transparent background.
          Shape::ColourRGBA processed_pixel_colour{0.f, 0.f, 0.f, 0.f};

          // Iterate through all shapes (already sorted by Z) for the current
          // pixel.
          for (const auto *shape : sorted_shapes) {
            if (shape->is_inside(px, py)) {
              // If the pixel is inside the shape, blend its color.
              processed_pixel_colour =
                  Renderer::blend(processed_pixel_colour, shape->get_colour());
            }
          }
          image.set_pixel(x, y, Renderer::convert_to_rgba8(processed_pixel_colour));
        }
      }
    }
  }
}
