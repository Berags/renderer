//
// Created by jacopo on 11/1/25.
//

#include "renderer/SpatialGridParallelRenderer.h"

#include <algorithm>
#include <iostream>
#include <omp.h>
#include <ranges>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void Renderer::SpatialGridParallelRenderer::render(Image &image,
                                                   const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    const auto width = image.getWidth();
    const auto height = image.getHeight();

    // Define tile size for the spatial grid.
    constexpr uint16_t TILE_SIZE = 32;
    const uint16_t numberOfTilesX = (width + TILE_SIZE - 1) / TILE_SIZE;
    const uint16_t numberOfTilesY = (height + TILE_SIZE - 1) / TILE_SIZE;
    const size_t totalTiles = static_cast<size_t>(numberOfTilesX) * numberOfTilesY;

    // A RenderItem is a flattened representation of a shape, optimized for rendering.
    std::vector<RenderItem> renderList(shapes.size());
    // The spatial index is a grid where each cell (tile) contains a list of shapes that overlap it.
    std::vector<std::vector<const RenderItem *> > spatialIndex(totalTiles);

    // Initialize one lock per tile for thread-safe access to the spatial index.
    std::vector<omp_lock_t> tileLocks(totalTiles);
    for (size_t i = 0; i < totalTiles; i++) {
        omp_init_lock(&tileLocks[i]);
    }

    // This parallel loop populates the spatial index. Each thread processes a subset of shapes.
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
        const int32_t pixelXMax = std::min(static_cast<int32_t>(width) - 1,
                                           static_cast<int32_t>(std::floor(xMax - 0.5f)));
        const int32_t pixelYMax = std::min(static_cast<int32_t>(height) - 1,
                                           static_cast<int32_t>(std::floor(yMax - 0.5f)));

        // Continue only if the shape is actually visible
        if (pixelXMin > pixelXMax || pixelYMin > pixelYMax) {
            continue;
        }

        // Determine the range of tiles that the shape's bounding box overlaps.
        const uint16_t tileXStart = pixelXMin / TILE_SIZE;
        const uint16_t tileYStart = pixelYMin / TILE_SIZE;

        // Ensure the last pixel is included in the correct tile, even at boundaries.
        const uint16_t tileXEnd = std::min(static_cast<uint16_t>(pixelXMax / TILE_SIZE),
                                           static_cast<uint16_t>(numberOfTilesX - 1));
        const uint16_t tileYEnd = std::min(static_cast<uint16_t>(pixelYMax / TILE_SIZE),
                                           static_cast<uint16_t>(numberOfTilesY - 1));

        // Add this shape to all tiles it overlaps. A lock is used for each tile to ensure thread safety.
        for (uint16_t ty = tileYStart; ty <= tileYEnd; ty++) {
            for (uint16_t tx = tileXStart; tx <= tileXEnd; tx++) {
                const size_t tileIndex = static_cast<size_t>(ty) * numberOfTilesX + tx;

                omp_set_lock(&tileLocks[tileIndex]);
                spatialIndex[tileIndex].push_back(&item);
                omp_unset_lock(&tileLocks[tileIndex]);
            }
        }
    }

    // Clean up the locks.
    for (size_t i = 0; i < totalTiles; i++) {
        omp_destroy_lock(&tileLocks[i]);
    }

    // This parallel loop processes each tile independently.
    // `collapse(2)` flattens the nested loops over tiles into a single parallelized loop.
    #pragma omp parallel for collapse(2)
    for (uint16_t tileIndexY = 0; tileIndexY < numberOfTilesY; tileIndexY++) {
        for (uint16_t tileIndexX = 0; tileIndexX < numberOfTilesX; tileIndexX++) {
            const size_t tileIndex = static_cast<size_t>(tileIndexY) * numberOfTilesX + tileIndexX;
            std::vector<const RenderItem *> &localShapes = spatialIndex[tileIndex];

            if (localShapes.empty()) {
                continue;
            }

            // Sort shapes within the tile by Z-index to handle occlusion correctly.
            std::ranges::stable_sort(localShapes, compareZ);

            // Convert the list of shapes (Array of Structures) into a Structure of Arrays (SoA)
            // for better cache performance and to enable SIMD optimizations.
            TileRenderDataSoA tileDataSoA{};
            for (const auto *itemPtr: localShapes) {
                if (const auto &item = *itemPtr; item.type == RenderItem::CIRCLE) {
                    tileDataSoA.circlesX.push_back(item.p1);
                    tileDataSoA.circlesY.push_back(item.p2);
                    tileDataSoA.circlesRadiusSquared.push_back(item.p3);
                    tileDataSoA.circlesColour.push_back(item.colour);
                    tileDataSoA.circlesId.push_back(item.id);
                    tileDataSoA.circlesZ.push_back(item.z);
                } else {
                    tileDataSoA.rectanglesXMin.push_back(item.p1);
                    tileDataSoA.rectanglesYMin.push_back(item.p2);
                    tileDataSoA.rectanglesXMax.push_back(item.p3);
                    tileDataSoA.rectanglesYMax.push_back(item.p4);
                    tileDataSoA.rectanglesColour.push_back(item.colour);
                    tileDataSoA.rectanglesId.push_back(item.id);
                    tileDataSoA.rectanglesZ.push_back(item.z);
                }
            }

            const uint16_t pixelStartY = tileIndexY * TILE_SIZE;
            const uint16_t pixelStartX = tileIndexX * TILE_SIZE;
            const uint16_t pixelEndY = std::min(static_cast<uint16_t>(pixelStartY + TILE_SIZE), height);
            const uint16_t pixelEndX = std::min(static_cast<uint16_t>(pixelStartX + TILE_SIZE), width);

            std::vector<bool> circlesInsidePixelMask(tileDataSoA.circlesX.size());
            std::vector<bool> rectanglesInsidePixelMask(tileDataSoA.rectanglesXMin.size());
            std::vector<std::pair<std::pair<uint8_t, size_t>, Shape::ColourRGBA> > shapesInPixel;
            shapesInPixel.reserve(localShapes.size());

            // Iterate over each pixel in the tile.
            for (uint16_t y = pixelStartY; y < pixelEndY; y++) {
                const float pixelCenterY = static_cast<float>(y) + 0.5f;

                for (uint16_t x = pixelStartX; x < pixelEndX; x++) {
                    const float pixelCenterX = static_cast<float>(x) + 0.5f;
                    shapesInPixel.clear();

                    // Use SIMD to quickly check which circles contain the current pixel center.
                    #pragma omp simd
                    for (size_t i = 0; i < tileDataSoA.circlesX.size(); ++i) {
                        const float dx = tileDataSoA.circlesX[i] - pixelCenterX;
                        const float dy = tileDataSoA.circlesY[i] - pixelCenterY;
                        circlesInsidePixelMask[i] = (dx * dx + dy * dy) <= tileDataSoA.circlesRadiusSquared[i];
                    }

                    // Use SIMD to quickly check which rectangles contain the current pixel center.
                    #pragma omp simd
                    for (size_t i = 0; i < tileDataSoA.rectanglesXMin.size(); ++i) {
                        rectanglesInsidePixelMask[i] = (pixelCenterX >= tileDataSoA.rectanglesXMin[i] && pixelCenterX <=
                                                        tileDataSoA.rectanglesXMax[i] &&
                                                        pixelCenterY >= tileDataSoA.rectanglesYMin[i] && pixelCenterY <=
                                                        tileDataSoA.rectanglesYMax[i]);
                    }

                    for (size_t i = 0; i < circlesInsidePixelMask.size(); ++i) {
                        if (circlesInsidePixelMask[i]) {
                            shapesInPixel.emplace_back(
                                std::make_pair(tileDataSoA.circlesZ[i], tileDataSoA.circlesId[i]),
                                tileDataSoA.circlesColour[i]
                            );
                        }
                    }

                    for (size_t i = 0; i < rectanglesInsidePixelMask.size(); ++i) {
                        if (rectanglesInsidePixelMask[i]) {
                            shapesInPixel.emplace_back(
                                std::make_pair(tileDataSoA.rectanglesZ[i], tileDataSoA.rectanglesId[i]),
                                tileDataSoA.rectanglesColour[i]
                            );
                        }
                    }

                    // Sort the shapes covering the pixel by Z-index to prepare for blending.
                    std::ranges::stable_sort(shapesInPixel, [](auto const &a, auto const &b) {
                        return std::tie(a.first.first, a.first.second) < std::tie(b.first.first, b.first.second);
                    });

                    // Blend the colors of all shapes covering the pixel, from back to front.
                    Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f};
                    for (const auto &val: shapesInPixel | std::views::values) {
                        processedPixelColour = Renderer::blend(processedPixelColour, val);
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
