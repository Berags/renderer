//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_UTILS_H
#define RENDERER_UTILS_H
#include <memory>
#include <random>
#include <vector>

#include "Image.h"
#include "shape/Circle.h"
#include "shape/IShape.h"
#include "shape/Rectangle.h"

namespace Utils {
    /**
     * Creates `n` random shapes (circles or rectangles) and appends them to the provided `shapes` vector.
     *
     * @param shapes Reference to a vector of unique pointers to IShape objects. New shapes will be added to this vector.
     * @param image Reference to an Image object, used to determine the valid ranges for shape positions.
     * @param n The number of shapes to create.
     *
     * Behavior:
     * - For each shape, randomly chooses between a circle and a rectangle (50% chance each).
     * - Position (x, y): Randomly chosen within the image dimensions ([0, width-1], [0, height-1]).
     * - z: Randomly chosen in [0, 254].
     * - Color: Each channel (R, G, B, A) in [0.0, 1.0].
     * - Circle radius: [20, 119].
     * - Rectangle length and width: [20, 169].
     */
    inline void createShapes(std::vector<std::unique_ptr<Shape::IShape> > &shapes, const Image &image,
                             const uint16_t n) {
        // Using a fixed seed for reproducibility
        static std::mt19937 gen(232710);

        const uint16_t width = image.getWidth();
        const uint16_t height = image.getHeight();

        std::uniform_int_distribution<uint16_t> xDistribution(0, width - 1);
        std::uniform_int_distribution<uint16_t> yDistribution(0, height - 1);
        std::uniform_int_distribution<uint8_t> zDistribution(0, 255);
        std::uniform_real_distribution colourDistribution(0.0f, 1.0f);
        std::uniform_int_distribution typeDistribution(0, 1);

        std::uniform_int_distribution<uint16_t> circleRadiusDistribution(20, 119);
        std::uniform_int_distribution<uint16_t> rectangleLengthDistribution(20, 169);
        std::uniform_int_distribution<uint16_t> rectangleWidthDistribution(20, 169);

        for (uint16_t i = 0; i < n; ++i) {
            // 50% chance to create a circle or rectangle
            if (typeDistribution(gen) == 0) {
                shapes.push_back(Shape::Circle::Builder()
                    .x(xDistribution(gen))
                    .y(yDistribution(gen))
                    .z(zDistribution(gen))
                    .radius(circleRadiusDistribution(gen))
                    .colour({
                        colourDistribution(gen),
                        colourDistribution(gen),
                        colourDistribution(gen),
                        colourDistribution(gen)
                    })
                    .build());
            } else {
                shapes.push_back(Shape::Rectangle::Builder()
                    .x(xDistribution(gen))
                    .y(yDistribution(gen))
                    .z(zDistribution(gen))
                    .length(rectangleLengthDistribution(gen))
                    .width(rectangleWidthDistribution(gen))
                    .colour({
                        colourDistribution(gen),
                        colourDistribution(gen),
                        colourDistribution(gen),
                        colourDistribution(gen)
                    })
                    .build());
            }
        }
    }
}

#endif //RENDERER_UTILS_H
