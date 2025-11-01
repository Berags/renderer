//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_UTILS_H
#define RENDERER_UTILS_H
#include <memory>
#include <vector>

#include "shape/Circle.h"
#include "shape/IShape.h"
#include "shape/Rectangle.h"

namespace Utils {
    inline void createShapes(std::vector<std::unique_ptr<Shape::IShape> > &shapes, Image &image, uint16_t n) {
        // Example utility function to create n shapes (circles and rectangles)
        // generating them randomly
        srand(static_cast<unsigned int>(time(nullptr)));

        const uint16_t width = image.getWidth();
        const uint16_t height = image.getHeight();

        for (int i = 0; i < n; ++i) {
            // 50% chance to create a circle or rectangle
            if (rand() % 2 == 0) {
                shapes.push_back(Shape::Circle::Builder()
                    .x(static_cast<uint16_t>(rand() % width))
                    .y(static_cast<uint16_t>(rand() % height))
                    .z(static_cast<uint8_t>(rand() % 255))
                    .radius(static_cast<uint16_t>(20 + rand() % 100))
                    .colour({
                        static_cast<uint8_t>(rand() % 256),
                        static_cast<uint8_t>(rand() % 256),
                        static_cast<uint8_t>(rand() % 256),
                        ((rand() % 100) / 100.0f)
                    })
                    .build());
            } else {
                shapes.push_back(Shape::Rectangle::Builder()
                    .x(static_cast<uint16_t>(rand() % width))
                    .y(static_cast<uint16_t>(rand() % height))
                    .z(static_cast<uint8_t>(rand() % 255))
                    .length(static_cast<uint16_t>(20 + rand() % 150))
                    .width(static_cast<uint16_t>(20 + rand() % 150))
                    .colour({
                        static_cast<uint8_t>(rand() % 256),
                        static_cast<uint8_t>(rand() % 256),
                        static_cast<uint8_t>(rand() % 256),
                        ((rand() % 100) / 100.0f)
                    })
                    .build());
            }
        }
    }
}

#endif //RENDERER_UTILS_H
