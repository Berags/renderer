//
// Created by jacopo on 10/28/25.
//

#include "../../include/renderer/SimpleParallelRenderer.h"

#include <algorithm>

void Renderer::SimpleParallelRenderer::render(Image &image,
                                              const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    // Create a vector of raw pointers to shapes to allow sorting without moving ownership.
    std::vector<const Shape::IShape *> sorted_shapes;
    sorted_shapes.reserve(shapes.size());
    for (const auto &ptr: shapes) {
        sorted_shapes.push_back(ptr.get());
    }

    // Sort shapes by Z-index (ascending) to ensure correct alpha blending from back to front.
    std::ranges::stable_sort(sorted_shapes, std::ranges::less{}, &Shape::IShape::getZ);

    const auto width = image.getWidth();
    const auto height = image.getHeight();

    // Define the size of the tiles for tile-based rendering.
    constexpr uint16_t TILE_SIZE = 32;
    // Calculate the number of tiles needed to cover the image area.
    const uint16_t num_tiles_x = (width + TILE_SIZE - 1) / TILE_SIZE;
    const uint16_t num_tiles_y = (height + TILE_SIZE - 1) / TILE_SIZE;

    // Parallelize the rendering process over the tiles using OpenMP.
    // The `collapse(2)` clause flattens the nested loops over tiles into a single parallel loop.
#pragma omp parallel for collapse(2)
    for (uint16_t ty = 0; ty < num_tiles_y; ++ty) {
        for (uint16_t tx = 0; tx < num_tiles_x; ++tx) {
            // Determine the pixel boundaries for the current tile.
            const uint16_t y_start = ty * TILE_SIZE;
            const uint16_t x_start = tx * TILE_SIZE;
            const uint16_t y_end = std::min(static_cast<uint16_t>(y_start + TILE_SIZE), height);
            const uint16_t x_end = std::min(static_cast<uint16_t>(x_start + TILE_SIZE), width);

            // Iterate over each pixel within the current tile.
            for (uint16_t y = y_start; y < y_end; ++y) {
                for (uint16_t x = x_start; x < x_end; ++x) {
                    // Calculate the pixel's center coordinates.
                    const float px = static_cast<float>(x) + 0.5f;
                    const float py = static_cast<float>(y) + 0.5f;

                    // Initialize with a transparent background.
                    Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f};

                    // Iterate through all shapes (already sorted by Z) for the current pixel.
                    for (const auto *shape: sorted_shapes) {
                        if (shape->isInside(px, py)) {
                            // If the pixel is inside the shape, blend its color.
                            processedPixelColour = Renderer::blend(processedPixelColour, shape->getColour());
                        }
                    }
                    image.setPixel(x, y, Renderer::convertToRGBA8(processedPixelColour));
                }
            }
        }
    }
}
