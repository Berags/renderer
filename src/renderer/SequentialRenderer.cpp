//
// Created by jacopo on 10/28/25.
//

#include "../../include/renderer/SequentialRenderer.h"

#include <algorithm>

#include "shape/Circle.h"
#include "shape/Rectangle.h"

void Renderer::SequentialRenderer::render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    std::vector<const Shape::IShape *> sorted_shapes;
    sorted_shapes.reserve(shapes.size());
    for (const auto &ptr: shapes) {
        sorted_shapes.push_back(ptr.get());
    }

    // Sort shapes by Z-index (ascending) to render from back to front
    std::ranges::sort(sorted_shapes, std::ranges::less{}, &Shape::IShape::getZ);

    const auto width = image.getWidth();
    const auto height = image.getHeight();

    for (uint16_t y = 0; y < height; ++y) {
        for (uint16_t x = 0; x < width; ++x) {
            // Pixel center
            const float px = static_cast<float>(x) + 0.5f;
            const float py = static_cast<float>(y) + 0.5f;

            Shape::ColourRGBA processedPixelColour{0.f, 0.f, 0.f, 0.f}; // Start with transparent black

            // Iterate through shapes from back to front
            for (const auto *shape: sorted_shapes) {
                if (shape->isInside(px, py)) {
                    // Blend the shape's color with the current pixel color
                    processedPixelColour = Renderer::blend(processedPixelColour, shape->getColour());
                }
            }
            image.setPixel(x, y, Renderer::convertToRGBA8(processedPixelColour));
        }
    }
}
