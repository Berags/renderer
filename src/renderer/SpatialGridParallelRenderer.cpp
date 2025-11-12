//
// Created by jacopo on 11/1/25.
//

#include "renderer/SpatialGridParallelRenderer.h"

#include <omp.h>

#include <algorithm>
#include <iostream>
#include <ranges>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void Renderer::SpatialGridParallelRenderer::render(
    Image &image,
    const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
  const auto width = image.get_width();
  const auto height = image.get_height();

  // Define tile size for the spatial grid.
  constexpr uint16_t kTileSize = 32;
  const uint16_t num_tiles_x = (width + kTileSize - 1) / kTileSize;
  const uint16_t num_tiles_y = (height + kTileSize - 1) / kTileSize;
  const size_t total_tiles =
      static_cast<size_t>(num_tiles_x) * num_tiles_y;

  // A RenderItem is a flattened representation of a shape, optimized for
  // rendering.
  std::vector<RenderItem> render_list(shapes.size());
  // The spatial index is a grid where each cell (tile) contains a list of
  // shapes that overlap it.
  std::vector<std::vector<const RenderItem *> > spatial_index(total_tiles);

  // Initialize one lock per tile for thread-safe access to the spatial index.
  std::vector<omp_lock_t> tile_locks(total_tiles);
  for (size_t i = 0; i < total_tiles; i++) {
    omp_init_lock(&tile_locks[i]);
  }

  // This parallel loop populates the spatial index. Each thread processes a
  // subset of shapes.
  #pragma omp parallel for
  for (size_t i = 0; i < shapes.size(); i++) {
    const auto &shape = shapes[i];
    RenderItem &item = render_list[i];

    item.id = i;
    item.z = shape->get_z();
    item.colour = shape->get_colour();

    RenderItemVisitor visitor(item);
    shape->accept(visitor);

    float xMin, yMin, xMax, yMax;

    if (item.type == RenderItem::CIRCLE) {
      const auto radius = std::sqrt(item.p3);
      xMin = item.p1 - radius;
      yMin = item.p2 - radius;
      xMax = item.p1 + radius;
      yMax = item.p2 + radius;
    } else {
      xMin = item.p1;
      yMin = item.p2;
      xMax = item.p3;
      yMax = item.p4;
    }

    // Use ceil for min index, floor for max index.
    // Convert from continuous coordinates to discrete pixel indices.
    // Pixels have centers at integer + 0.5, so subtract 0.5 before ceil/floor
    // to account for the pixel center offset.
    const int32_t pixel_x_min =
        std::max(0, static_cast<int32_t>(std::ceil(xMin - 0.5f)));
    const int32_t pixel_y_min =
        std::max(0, static_cast<int32_t>(std::ceil(yMin - 0.5f)));
    const int32_t pixel_x_max =
        std::min(static_cast<int32_t>(width) - 1,
                 static_cast<int32_t>(std::floor(xMax - 0.5f)));
    const int32_t pixel_y_max =
        std::min(static_cast<int32_t>(height) - 1,
                 static_cast<int32_t>(std::floor(yMax - 0.5f)));

    // Continue only if the shape is actually visible
    if (pixel_x_min > pixel_x_max || pixel_y_min > pixel_y_max) {
      continue;
    }

    // Determine the range of tiles that the shape's bounding box overlaps.
    const uint16_t tile_x_start = pixel_x_min / kTileSize;
    const uint16_t tile_y_start = pixel_y_min / kTileSize;

    // Ensure the last pixel is included in the correct tile, even at
    // boundaries.
    const uint16_t tile_x_end =
        std::min(static_cast<uint16_t>(pixel_x_max / kTileSize),
                 static_cast<uint16_t>(num_tiles_x - 1));
    const uint16_t tile_y_end =
        std::min(static_cast<uint16_t>(pixel_y_max / kTileSize),
                 static_cast<uint16_t>(num_tiles_y - 1));

    // Add this shape to all tiles it overlaps. A lock is used for each tile to
    // ensure thread safety.
    for (uint16_t ty = tile_y_start; ty <= tile_y_end; ty++) {
      for (uint16_t tx = tile_x_start; tx <= tile_x_end; tx++) {
        const size_t tile_index = static_cast<size_t>(ty) * num_tiles_x + tx;

        omp_set_lock(&tile_locks[tile_index]);
        spatial_index[tile_index].push_back(&item);
        omp_unset_lock(&tile_locks[tile_index]);
      }
    }
  }

  // Clean up the locks.
  for (size_t i = 0; i < total_tiles; i++) {
    omp_destroy_lock(&tile_locks[i]);
  }

  // This parallel loop processes each tile independently.
  // `collapse(2)` flattens the nested loops over tiles into a single parallelized
  // loop.
  #pragma omp parallel for collapse(2)
  for (uint16_t tile_index_y = 0; tile_index_y < num_tiles_y; tile_index_y++) {
    for (uint16_t tile_index_x = 0; tile_index_x < num_tiles_x; tile_index_x++) {
      const size_t tile_index =
          static_cast<size_t>(tile_index_y) * num_tiles_x + tile_index_x;
      std::vector<const RenderItem *> &local_shapes = spatial_index[tile_index];

      if (local_shapes.empty()) {
        continue;
      }

      // Sort shapes within the tile by Z-index to handle occlusion correctly.
      std::ranges::stable_sort(local_shapes, compare_z);

      // Convert the list of shapes (Array of Structures) into a Structure of
      // Arrays (SoA) for better cache performance and to enable SIMD
      // optimizations.
      TileRenderDataSoA tile_data{};
      for (const auto *item_pointer : local_shapes) {
        if (const auto &[z, type, id, colour, p1, p2, p3, p4] = *item_pointer;
            type == RenderItem::CIRCLE) {
          tile_data.circles_x.push_back(p1);
          tile_data.circles_y.push_back(p2);
          tile_data.circles_radius_squared.push_back(p3);
          tile_data.circles_colour.push_back(colour);
          tile_data.circles_id.push_back(id);
          tile_data.circles_z.push_back(z);
        } else {
          tile_data.rectangles_x_min.push_back(p1);
          tile_data.rectangles_y_min.push_back(p2);
          tile_data.rectangles_x_max.push_back(p3);
          tile_data.rectangles_y_max.push_back(p4);
          tile_data.rectangles_colour.push_back(colour);
          tile_data.rectangles_id.push_back(id);
          tile_data.rectangles_z.push_back(z);
        }
      }

      const uint16_t pixel_start_y = tile_index_y * kTileSize;
      const uint16_t pixel_start_x = tile_index_x * kTileSize;
      const uint16_t pixel_end_y =
          std::min(static_cast<uint16_t>(pixel_start_y + kTileSize), height);
      const uint16_t pixel_end_x =
          std::min(static_cast<uint16_t>(pixel_start_x + kTileSize), width);

      std::vector<bool> circles_inside_pixel_mask(tile_data.circles_x.size());
      std::vector<bool> rectangles_inside_pixel_mask(
          tile_data.rectangles_x_min.size());
      std::vector<Shape::ColourRGBA> shapes_in_pixel_colours;
      shapes_in_pixel_colours.reserve(local_shapes.size());

      // Iterate over each pixel in the tile.
      for (uint16_t y = pixel_start_y; y < pixel_end_y; y++) {
        const float pixel_center_y = static_cast<float>(y) + 0.5f;

        for (uint16_t x = pixel_start_x; x < pixel_end_x; x++) {
          const float pixel_center_x = static_cast<float>(x) + 0.5f;
          shapes_in_pixel_colours.clear();

          // Use SIMD to quickly check which circles contain the current pixel center.
          #pragma omp simd
          for (size_t i = 0; i < tile_data.circles_x.size(); ++i) {
            const float dx = tile_data.circles_x[i] - pixel_center_x;
            const float dy = tile_data.circles_y[i] - pixel_center_y;
            circles_inside_pixel_mask[i] =
                (dx * dx + dy * dy) <= tile_data.circles_radius_squared[i];
          }

          // Use SIMD to quickly check which rectangles contain the current pixel center.
          #pragma omp simd
          for (size_t i = 0; i < tile_data.rectangles_x_min.size(); ++i) {
            rectangles_inside_pixel_mask[i] =
                (pixel_center_x >= tile_data.rectangles_x_min[i] &&
                 pixel_center_x <= tile_data.rectangles_x_max[i] &&
                 pixel_center_y >= tile_data.rectangles_y_min[i] &&
                 pixel_center_y <= tile_data.rectangles_y_max[i]);
          }

          merge_colours(tile_data, circles_inside_pixel_mask,
                        rectangles_inside_pixel_mask, shapes_in_pixel_colours);

          // Blend the colors of all shapes covering the pixel, from back to
          // front.
          Shape::ColourRGBA processed_pixel_colour{0.f, 0.f, 0.f, 0.f};
          for (const auto &colour : shapes_in_pixel_colours) {
            processed_pixel_colour =
                Renderer::blend(processed_pixel_colour, colour);
          }

          image.set_pixel(x, y,
                          Renderer::convert_to_rgba8(processed_pixel_colour));
        }
      }
    }
  }
}

Renderer::SpatialGridParallelRenderer::RenderItemVisitor::RenderItemVisitor(
    RenderItem &item)
    : item_(item) {}

void Renderer::SpatialGridParallelRenderer::RenderItemVisitor::visit(
    const Shape::Circle &c) {
  item_.type = RenderItem::CIRCLE;
  item_.p1 = static_cast<float>(c.get_x());  // center_x
  item_.p2 = static_cast<float>(c.get_y());  // center_y
  const auto radius = static_cast<float>(c.get_radius());
  item_.p3 = radius * radius;  // radius_sq
}

void Renderer::SpatialGridParallelRenderer::RenderItemVisitor::visit(
    const Shape::Rectangle &r) {
  item_.type = RenderItem::RECTANGLE;
  const float half_length = static_cast<float>(r.get_length()) / 2.0f;
  const float half_width = static_cast<float>(r.get_width()) / 2.0f;
  item_.p1 = static_cast<float>(r.get_x()) - half_length;  // xMin
  item_.p2 = static_cast<float>(r.get_y()) - half_width;   // yMin
  item_.p3 = static_cast<float>(r.get_x()) + half_length;  // xMax
  item_.p4 = static_cast<float>(r.get_y()) + half_width;   // yMax
}

void Renderer::SpatialGridParallelRenderer::merge_colours(
    TileRenderDataSoA &tile_data, std::vector<bool> &circles_inside_pixel_mask,
    std::vector<bool> &rectangles_inside_pixel_mask,
    std::vector<Shape::ColourRGBA> &shapes_in_pixel_colours) {
  // Merge the sorted lists of circles and rectangles that cover the pixel.
  size_t circle_index = 0;
  size_t rectangles_index = 0;
  while (circle_index < circles_inside_pixel_mask.size() &&
         rectangles_index < rectangles_inside_pixel_mask.size()) {
    // Find next intersecting circle
    while (circle_index < circles_inside_pixel_mask.size() &&
           !circles_inside_pixel_mask[circle_index]) {
      circle_index++;
    }
    // Find next intersecting rectangle
    while (rectangles_index < rectangles_inside_pixel_mask.size() &&
           !rectangles_inside_pixel_mask[rectangles_index]) {
      rectangles_index++;
    }

    if (circle_index < circles_inside_pixel_mask.size() &&
        rectangles_index < rectangles_inside_pixel_mask.size()) {
      if (std::tie(tile_data.circles_z[circle_index],
                   tile_data.circles_id[circle_index]) <
          std::tie(tile_data.rectangles_z[rectangles_index],
                   tile_data.rectangles_id[rectangles_index])) {
        shapes_in_pixel_colours.push_back(
            tile_data.circles_colour[circle_index++]);
      } else {
        shapes_in_pixel_colours.push_back(
            tile_data.rectangles_colour[rectangles_index++]);
      }
    }
  }

  // Add remaining intersecting circles
  while (circle_index < circles_inside_pixel_mask.size()) {
    if (circles_inside_pixel_mask[circle_index]) {
      shapes_in_pixel_colours.push_back(tile_data.circles_colour[circle_index]);
    }
    circle_index++;
  }

  // Add remaining intersecting rectangles
  while (rectangles_index < rectangles_inside_pixel_mask.size()) {
    if (rectangles_inside_pixel_mask[rectangles_index]) {
      shapes_in_pixel_colours.push_back(
          tile_data.rectangles_colour[rectangles_index]);
    }
    rectangles_index++;
  }
}
