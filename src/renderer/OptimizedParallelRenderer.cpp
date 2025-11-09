//
// Created by jacopo on 11/1/25.
//

#include "renderer/OptimizedParallelRenderer.h"

#include <algorithm>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void Renderer::OptimizedParallelRenderer::render(Image &image,
                                                 const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    // Convert polymorphic shapes to flat RenderItem structs for better cache locality and vectorization
    std::vector<RenderItem> renderList;
    renderList.reserve(shapes.size());

    // This adds a O(N) preprocessing step, but allows better performance in the inner loops
    size_t i = 0;
    for (const auto &shapePointer: shapes) {
        RenderItem item{};
        item.id = i++;
        item.z = shapePointer->getZ();
        item.colour = shapePointer->getColour();

        if (const auto *c = dynamic_cast<const Shape::Circle *>(shapePointer.get())) {
            item.type = RenderItem::CIRCLE;
            item.p1 = static_cast<float>(c->getX());
            item.p2 = static_cast<float>(c->getY());
            // Store radius squared to avoid sqrt in inner loop
            item.p3 = static_cast<float>(c->getRadius()) * static_cast<float>(c->getRadius());
        } else if (const auto *r = dynamic_cast<const Shape::Rectangle *>(shapePointer.get())) {
            item.type = RenderItem::RECTANGLE;
            const float halfLength = static_cast<float>(r->getLength()) / 2.0f;
            const float halfWidth = static_cast<float>(r->getWidth()) / 2.0f;
            item.p1 = static_cast<float>(r->getX()) - halfLength; // x_min
            item.p2 = static_cast<float>(r->getY()) - halfWidth; // y_min
            item.p3 = static_cast<float>(r->getX()) + halfLength; // x_max
            item.p4 = static_cast<float>(r->getY()) + halfWidth; // y_max
        }
        renderList.push_back(item);
    }

    // Sort shapes by Z-axis for correct transparency/alpha blending (back-to-front)
    std::ranges::stable_sort(renderList, compareZ);

    const auto width = image.getWidth();
    const auto height = image.getHeight();

    // Tile-based rendering improves cache locality and enables parallel processing
    constexpr uint16_t TILE_SIZE = 32;
    const uint16_t numberOfTilesX = (width + TILE_SIZE - 1) / TILE_SIZE;
    const uint16_t numberOfTilesY = (height + TILE_SIZE - 1) / TILE_SIZE;

    // Parallelize over tiles using OpenMP (collapse(2) parallelizes both nested loops)
    #pragma omp parallel for collapse(2)
    for (uint16_t ty = 0; ty < numberOfTilesY; ty++) {
        for (uint16_t tx = 0; tx < numberOfTilesX; tx++) {
            const uint16_t tileYStart = ty * TILE_SIZE;
            const uint16_t tileXStart = tx * TILE_SIZE;
            const uint16_t tileYEnd = std::min(static_cast<uint16_t>(tileYStart + TILE_SIZE), height);
            const uint16_t tileXEnd = std::min(static_cast<uint16_t>(tileXStart + TILE_SIZE), width);

            for (uint16_t y = tileYStart; y < tileYEnd; y++) {
                // Pixel center is at +0.5 offset
                const float py = static_cast<float>(y) + 0.5f;

                for (uint16_t x = tileXStart; x < tileXEnd; x++) {
                    const float px = static_cast<float>(x) + 0.5f;

                    // Start with transparent background
                    Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f};

                    // Process shapes in back-to-front order for correct alpha blending
                    for (const auto &item: renderList) {
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
                    image.setPixel(x, y, Renderer::convertToRGBA8(processedPixelColour));
                }
            }
        }
    }
}
