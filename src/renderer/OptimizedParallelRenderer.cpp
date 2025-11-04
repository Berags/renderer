//
// Created by jacopo on 11/1/25.
//

#include "../../include/renderer/OptimizedParallelRenderer.h"

#include <algorithm>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void OptimizedParallelRenderer::render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    // Convert polymorphic shapes to flat RenderItem structs for better cache locality and vectorization
    std::vector<RenderItem> render_list;
    render_list.reserve(shapes.size());

    // This adds a O(N) preprocessing step, but allows better performance in the inner loops
    for (const auto &s_ptr: shapes) {
        RenderItem item{};
        item.z = s_ptr->getZ();
        item.colour = s_ptr->getColour();

        if (const auto *c = dynamic_cast<const Shape::Circle *>(s_ptr.get())) {
            item.type = RenderItem::CIRCLE;
            item.p1 = static_cast<float>(c->getX());
            item.p2 = static_cast<float>(c->getY());
            // Store radius squared to avoid sqrt in inner loop
            item.p3 = static_cast<float>(c->getRadius() * c->getRadius());
        } else if (const auto *r = dynamic_cast<const Shape::Rectangle *>(s_ptr.get())) {
            item.type = RenderItem::RECTANGLE;
            const float half_len = static_cast<float>(r->getLength()) / 2.0f;
            const float half_wid = static_cast<float>(r->getWidth()) / 2.0f;
            item.p1 = static_cast<float>(r->getX()) - half_len; // x_min
            item.p2 = static_cast<float>(r->getY()) - half_wid; // y_min
            item.p3 = static_cast<float>(r->getX()) + half_len; // x_max
            item.p4 = static_cast<float>(r->getY()) + half_wid; // y_max
        }
        render_list.push_back(item);
    }

    // Sort shapes by Z-axis for correct transparency/alpha blending (back-to-front)
    std::ranges::sort(render_list, compareRenderItemZ);

    const auto width = image.getWidth();
    const auto height = image.getHeight();

    // Tile-based rendering improves cache locality and enables parallel processing
    constexpr uint16_t TILE_SIZE = 32;
    const uint16_t num_tiles_x = (width + TILE_SIZE - 1) / TILE_SIZE;
    const uint16_t num_tiles_y = (height + TILE_SIZE - 1) / TILE_SIZE;

    // Parallelize over tiles using OpenMP (collapse(2) parallelizes both nested loops)
    #pragma omp parallel for collapse(2)
    for (uint16_t ty = 0; ty < num_tiles_y; ++ty) {
        for (uint16_t tx = 0; tx < num_tiles_x; ++tx) {
            const uint16_t y_start = ty * TILE_SIZE;
            const uint16_t x_start = tx * TILE_SIZE;
            const uint16_t y_end = std::min(static_cast<uint16_t>(y_start + TILE_SIZE), height);
            const uint16_t x_end = std::min(static_cast<uint16_t>(x_start + TILE_SIZE), width);

            for (uint16_t y = y_start; y < y_end; ++y) {
                // Pixel center is at +0.5 offset
                const float py = static_cast<float>(y) + 0.5f;

                // SIMD vectorization for pixel-level parallelism within a row
                #pragma omp simd
                for (uint16_t x = x_start; x < x_end; ++x) {
                    const float px = static_cast<float>(x) + 0.5f;

                    // Start with transparent background
                    Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f};

                    // Process shapes in back-to-front order for correct alpha blending
                    for (const auto &item: render_list) {
                        bool inside = false;

                        if (item.type == RenderItem::CIRCLE) {
                            // Circle containment test: distance squared <= radius squared
                            const float dx = item.p1 - px;
                            const float dy = item.p2 - py;
                            inside = (dx * dx + dy * dy) <= item.p3;
                        } else {
                            // Rectangle containment test: point within bounding box
                            inside = (px >= item.p1 && px <= item.p3 &&
                                      py >= item.p2 && py <= item.p4);
                        }

                        if (inside) {
                            // Alpha blend the shape color on top of the current pixel color
                            processedPixelColour = Renderer::blend(processedPixelColour, item.colour);
                        }
                    }
                    image.set_pixel(x, y, Renderer::convertToRGBA8(processedPixelColour));
                }
            }
        }
    }
}
