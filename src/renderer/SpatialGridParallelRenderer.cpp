//
// Created by jacopo on 11/1/25.
//

#include "renderer/SpatialGridParallelRenderer.h"

#include <algorithm>
#include <iostream>
#include <omp.h>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void Renderer::SpatialGridParallelRenderer::render(Image &image,
                                                   const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    const auto width = image.getWidth();
    const auto height = image.getHeight();

    constexpr uint16_t TILE_SIZE = 32;
    const uint16_t numberOfTilesX = (width + TILE_SIZE - 1) / TILE_SIZE;
    const uint16_t numberOfTilesY = (height + TILE_SIZE - 1) / TILE_SIZE;
    const size_t totalTiles = static_cast<size_t>(numberOfTilesX) * numberOfTilesY;

    std::vector<RenderItem> renderList(shapes.size());
    std::vector<std::vector<const RenderItem *> > spatialIndex(totalTiles);

    std::vector<omp_lock_t> tileLocks(totalTiles);
    for (size_t i = 0; i < totalTiles; ++i) {
        omp_init_lock(&tileLocks[i]);
    }

#pragma omp parallel for
    for (size_t i = 0; i < shapes.size(); ++i) {
        const auto &shape = shapes[i];
        RenderItem &item = renderList[i];

        item.z = shape->getZ();
        item.colour = shape->getColour();

        float xMin, yMin, xMax, yMax;

        if (const auto *c = dynamic_cast<const Shape::Circle *>(shape.get())) {
            item.type = RenderItem::CIRCLE;
            item.p1 = static_cast<float>(c->getX()); // center_x
            item.p2 = static_cast<float>(c->getY()); // center_y
            const auto radius = static_cast<float>(c->getRadius());
            item.p3 = radius * radius; // radius_sq

            xMin = item.p1 - radius;
            yMin = item.p2 - radius;
            xMax = item.p1 + radius;
            yMax = item.p2 + radius;
        } else if (const auto *r = dynamic_cast<const Shape::Rectangle *>(shape.get())) {
            item.type = RenderItem::RECTANGLE;
            const float halfLength = static_cast<float>(r->getLength()) / 2.0f;
            const float halfWidth = static_cast<float>(r->getWidth()) / 2.0f;
            item.p1 = static_cast<float>(r->getX()) - halfLength; // xMin
            item.p2 = static_cast<float>(r->getY()) - halfWidth; // yMin
            item.p3 = static_cast<float>(r->getX()) + halfLength; // xMax
            item.p4 = static_cast<float>(r->getY()) + halfWidth; // yMax

            xMin = item.p1;
            yMin = item.p2;
            xMax = item.p3;
            yMax = item.p4;
        }

        // Use ceil for min index, floor for max index.
        const int32_t pixelXMin = std::max(0, static_cast<int32_t>(std::ceil(xMin - 0.5f)));
        const int32_t pixelYMin = std::max(0, static_cast<int32_t>(std::ceil(yMin - 0.5f)));
        const int32_t pixelXMax = std::min((width - 1),
                                           static_cast<int32_t>(std::floor(xMax - 0.5f)));
        const int32_t pixelYMax = std::min((height - 1),
                                           static_cast<int32_t>(std::floor(yMax - 0.5f)));

        // Continue only if the shape is actually visible
        if (pixelXMin > pixelXMax || pixelYMin > pixelYMax) {
            continue;
        }

        // Tile range that covers this pixel range
        const uint16_t tileXStart = pixelXMin / TILE_SIZE;
        const uint16_t tileYStart = pixelYMin / TILE_SIZE;
        const uint16_t tileXEnd = std::min(pixelXMax / TILE_SIZE, (numberOfTilesX - 1));
        const uint16_t tileYEnd = std::min(pixelYMax / TILE_SIZE, (numberOfTilesY - 1));

        // Add this shape to all tiles it overlaps.
        for (uint16_t ty = tileYStart; ty <= tileYEnd; ++ty) {
            for (uint16_t tx = tileXStart; tx <= tileXEnd; ++tx) {
                const size_t tileIndex = static_cast<size_t>(ty) * numberOfTilesX + tx;

                omp_set_lock(&tileLocks[tileIndex]);
                spatialIndex[tileIndex].push_back(&item);
                omp_unset_lock(&tileLocks[tileIndex]);
            }
        }
    }

    for (size_t i = 0; i < totalTiles; ++i) {
        omp_destroy_lock(&tileLocks[i]);
    }

    // Parallelize over tiles using OpenMP (collapse(2) parallelizes both nested loops)
    #pragma omp parallel for collapse(2)
    for (uint16_t ty = 0; ty < numberOfTilesY; ++ty) {
        for (uint16_t tx = 0; tx < numberOfTilesX; ++tx) {
            size_t tileIndex = static_cast<size_t>(ty) * numberOfTilesX + tx;
            std::vector<const RenderItem *> localShapes = spatialIndex[tileIndex];

            if (localShapes.empty()) {
                continue;
            }

            std::ranges::stable_sort(localShapes, compareZ);

            const uint16_t yStart = ty * TILE_SIZE;
            const uint16_t xStart = tx * TILE_SIZE;
            const uint16_t yEnd = std::min(static_cast<uint16_t>(yStart + TILE_SIZE), height);
            const uint16_t xEnd = std::min(static_cast<uint16_t>(xStart + TILE_SIZE), width);

            for (uint16_t y = yStart; y < yEnd; ++y) {
                const float py = static_cast<float>(y) + 0.5f;

                #pragma omp simd
                for (uint16_t x = xStart; x < xEnd; ++x) {
                    const float px = static_cast<float>(x) + 0.5f;
                    Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f};

                    for (const auto *itemPtr: localShapes) {
                        const auto &item = *itemPtr;
                        bool inside = false;

                        if (item.type == RenderItem::CIRCLE) {
                            const float dx = item.p1 - px;
                            const float dy = item.p2 - py;
                            inside = (dx * dx + dy * dy) <= item.p3;
                        } else {
                            inside = (px >= item.p1 && px <= item.p3 &&
                                      py >= item.p2 && py <= item.p4);
                        }

                        if (inside) {
                            processedPixelColour = Renderer::blend(processedPixelColour, item.colour);
                        }
                    }
                    image.setPixel(x, y, Renderer::convertToRGBA8(processedPixelColour));
                }
            }
        }
    }
}
