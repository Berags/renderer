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
     * - Color: Each channel (R, G, B) in [0, 255], alpha in [0.0, 1.0].
     * - Circle radius: [20, 119].
     * - Rectangle length and width: [20, 169].
     */
    inline void createShapes(std::vector<std::unique_ptr<Shape::IShape> > &shapes, const Image &image,
                             const uint16_t n) {
        // Modern C++ random number generation
        static std::random_device rd;
        static std::mt19937 gen(rd());

        const uint16_t width = image.getWidth();
        const uint16_t height = image.getHeight();

        std::uniform_int_distribution<uint16_t> x_dist(0, width - 1);
        std::uniform_int_distribution<uint16_t> y_dist(0, height - 1);
        std::uniform_int_distribution<uint8_t> z_dist(0, 254);
        std::uniform_real_distribution<float> color_dist(0.0f, 1.0f);
        std::uniform_int_distribution<> type_dist(0, 1);

        std::uniform_int_distribution<uint16_t> circle_radius_dist(20, 119);
        std::uniform_int_distribution<uint16_t> rect_length_dist(20, 169);
        std::uniform_int_distribution<uint16_t> rect_width_dist(20, 169);

        for (uint16_t i = 0; i < n; ++i) {
            // 50% chance to create a circle or rectangle
            if (type_dist(gen) == 0) {
                shapes.push_back(Shape::Circle::Builder()
                    .x(x_dist(gen))
                    .y(y_dist(gen))
                    .z(z_dist(gen))
                    .radius(circle_radius_dist(gen))
                    .colour({
                        color_dist(gen),
                        color_dist(gen),
                        color_dist(gen),
                        color_dist(gen)
                    })
                    .build());
            } else {
                shapes.push_back(Shape::Rectangle::Builder()
                    .x(x_dist(gen))
                    .y(y_dist(gen))
                    .z(z_dist(gen))
                    .length(rect_length_dist(gen))
                    .width(rect_width_dist(gen))
                    .colour({
                        color_dist(gen),
                        color_dist(gen),
                        color_dist(gen),
                        color_dist(gen)
                    })
                    .build());
            }
        }
    }
}

#endif //RENDERER_UTILS_H
