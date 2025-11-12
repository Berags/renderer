//
// Created by jacopo on 11/1/25.
//

#include "renderer/OptimizedParallelRenderer.h"

#include <algorithm>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void Renderer::OptimizedParallelRenderer::render(
    Image &image,
    const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
  // Convert polymorphic shapes to flat RenderItem structs for better cache
  // locality and vectorization
  std::vector<RenderItem> render_list;
  render_list.reserve(shapes.size());

  // This adds a O(N) preprocessing step, but allows better performance in the
  // inner loops
  size_t i = 0;
  for (const auto &shape : shapes) {
    RenderItem item{};
    item.id = i++;
    item.z = shape->get_z();
    item.colour = shape->get_colour();

    RenderItemVisitor visitor(item);
    shape->accept(visitor);

    render_list.push_back(item);
  }

  // Sort shapes by Z-axis for correct transparency/alpha blending
  // (back-to-front)
  std::ranges::stable_sort(render_list, compare_z);

  const auto width = image.get_width();
  const auto height = image.get_height();

  // Tile-based rendering improves cache locality and enables parallel
  // processing
  constexpr uint16_t kTileSize = 32;
  const uint16_t num_tiles_x = (width + kTileSize - 1) / kTileSize;
  const uint16_t num_tiles_y = (height + kTileSize - 1) / kTileSize;

  // Parallelize over tiles using OpenMP (collapse(2) parallelizes both nested
  // loops)
  #pragma omp parallel for collapse(2)
  for (uint16_t ty = 0; ty < num_tiles_y; ty++) {
    for (uint16_t tx = 0; tx < num_tiles_x; tx++) {
      const uint16_t tile_y_start = ty * kTileSize;
      const uint16_t tile_x_start = tx * kTileSize;
      const uint16_t tile_y_end =
          std::min(static_cast<uint16_t>(tile_y_start + kTileSize), height);
      const uint16_t tile_x_end =
          std::min(static_cast<uint16_t>(tile_x_start + kTileSize), width);

      for (uint16_t y = tile_y_start; y < tile_y_end; y++) {
        // Pixel center is at +0.5 offset
        const float py = static_cast<float>(y) + 0.5f;

        #pragma omp simd
        for (uint16_t x = tile_x_start; x < tile_x_end; x++) {
          const float px = static_cast<float>(x) + 0.5f;

          // Start with transparent background
          Shape::ColourRGBA processed_pixel_colour{0.f, 0.f, 0.f, 0.f};

          // Process shapes in back-to-front order for correct alpha blending
          for (const auto &item : render_list) {
            bool inside = false;

            if (item.type == RenderItem::kCircle) {
              // Circle containment test: distance squared <= radius squared
              const float dx = item.p1 - px;
              const float dy = item.p2 - py;
              inside = (dx * dx + dy * dy) <= item.p3;
            } else {
              // Rectangle containment test: point within bounding box
              inside = (px >= item.p1 && px <= item.p3 && py >= item.p2 &&
                        py <= item.p4);
            }

            if (inside) {
              // Alpha blend the shape color on top of the current pixel color
              processed_pixel_colour =
                  Renderer::blend(processed_pixel_colour, item.colour);
            }
          }
          image.set_pixel(x, y, Renderer::convert_to_rgba8(processed_pixel_colour));
        }
      }
    }
  }
}
