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
    for (size_t i = 0; i < totalTiles; i++) {
        omp_init_lock(&tileLocks[i]);
    }

    #pragma omp parallel for
    for (size_t i = 0; i < shapes.size(); i++) {
        const auto &shape = shapes[i];
        RenderItem &item = renderList[i];

        item.id = i;
        item.z = shape->getZ();
        item.colour = shape->getColour();

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

        // Ensure the last pixel is included in the correct tile, even at boundaries.
        // When pixelXMax is at the edge of a tile boundary (e.g., pixelXMax = 63, TILE_SIZE = 32),
        // the pixel is actually within tile 1 (0-indexed).
        // If pixelXMax = 64, it should include tile 2.
        const uint16_t tileXEnd = std::min((pixelXMax + TILE_SIZE - 1) / TILE_SIZE, (numberOfTilesX - 1));
        const uint16_t tileYEnd = std::min((pixelYMax + TILE_SIZE - 1) / TILE_SIZE, (numberOfTilesY - 1));

        // Add this shape to all tiles it overlaps.
        for (uint16_t ty = tileYStart; ty <= tileYEnd; ty++) {
            for (uint16_t tx = tileXStart; tx <= tileXEnd; tx++) {
                const size_t tileIndex = static_cast<size_t>(ty) * numberOfTilesX + tx;

                omp_set_lock(&tileLocks[tileIndex]);
                spatialIndex[tileIndex].push_back(&item);
                omp_unset_lock(&tileLocks[tileIndex]);
            }
        }
    }

    for (size_t i = 0; i < totalTiles; i++) {
        omp_destroy_lock(&tileLocks[i]);
    }

    // Parallelize over tiles using OpenMP (collapse(2) parallelizes both nested loops)
    #pragma omp parallel for collapse(2)
    for (uint16_t tileIndexY = 0; tileIndexY < numberOfTilesY; tileIndexY++) {
        for (uint16_t tileIndexX = 0; tileIndexX < numberOfTilesX; tileIndexX++) {
            const size_t tileIndex = static_cast<size_t>(tileIndexY) * numberOfTilesX + tileIndexX;
            std::vector<const RenderItem *> &localShapes = spatialIndex[tileIndex];

            if (localShapes.empty()) {
                continue;
            }

            std::ranges::stable_sort(localShapes, compareZ);

            const uint16_t pixelStartY = tileIndexY * TILE_SIZE;
            const uint16_t pixelStartX = tileIndexX * TILE_SIZE;
            const uint16_t pixelEndY = std::min(static_cast<uint16_t>(pixelStartY + TILE_SIZE), height);
            const uint16_t pixelEndX = std::min(static_cast<uint16_t>(pixelStartX + TILE_SIZE), width);

            for (uint16_t y = pixelStartY; y < pixelEndY; y++) {
                const float pixelCenterY = static_cast<float>(y) + 0.5f;

                #pragma omp simd
                for (uint16_t x = pixelStartX; x < pixelEndX; x++) {
                    const float pixelCenterX = static_cast<float>(x) + 0.5f;
                    Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f};

                    for (const auto *itemPointer: localShapes) {
                        const auto &item = *itemPointer;
                        bool inside = false;

                        if (item.type == RenderItem::CIRCLE) {
                            const float dx = item.p1 - pixelCenterX;
                            const float dy = item.p2 - pixelCenterY;
                            inside = (dx * dx + dy * dy) <= item.p3;
                        } else {
                            inside = (pixelCenterX >= item.p1 && pixelCenterX <= item.p3 &&
                                      pixelCenterY >= item.p2 && pixelCenterY <= item.p4);
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

Renderer::SpatialGridParallelRenderer::RenderItemVisitor::RenderItemVisitor(RenderItem &item) : _item(item) {
}

void Renderer::SpatialGridParallelRenderer::RenderItemVisitor::visit(const Shape::Circle &c) {
    _item.type = RenderItem::CIRCLE;
    _item.p1 = static_cast<float>(c.getX()); // center_x
    _item.p2 = static_cast<float>(c.getY()); // center_y
    const auto radius = static_cast<float>(c.getRadius());
    _item.p3 = radius * radius; // radius_sq
}

void Renderer::SpatialGridParallelRenderer::RenderItemVisitor::visit(const Shape::Rectangle &r) {
    _item.type = RenderItem::RECTANGLE;
    const float halfLength = static_cast<float>(r.getLength()) / 2.0f;
    const float halfWidth = static_cast<float>(r.getWidth()) / 2.0f;
    _item.p1 = static_cast<float>(r.getX()) - halfLength; // xMin
    _item.p2 = static_cast<float>(r.getY()) - halfWidth; // yMin
    _item.p3 = static_cast<float>(r.getX()) + halfLength; // xMax
    _item.p4 = static_cast<float>(r.getY()) + halfWidth; // yMax
}
