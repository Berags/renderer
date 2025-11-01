//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_RECTANGLE_H
#define RENDERER_RECTANGLE_H
#include "IShape.h"

namespace Shape {
    class Rectangle : public IShape {
    public:
        class Builder {
        public:
            Builder() = default;

            Builder &x(uint16_t x);

            Builder &y(uint16_t y);

            Builder &z(uint8_t z);

            Builder &length(uint16_t length);

            Builder &width(uint16_t width);

            Builder &colour(const ColourRGBA &colour);

            [[nodiscard]] Rectangle build() const;

        private:
            uint16_t _x{0}, _y{0};
            uint8_t _z{0};
            uint16_t _length{0}, _width{0};
            ColourRGBA _colour{};
        };
    private:
        explicit Rectangle(uint16_t x, uint16_t y, uint8_t z, uint16_t length, uint16_t width, const ColourRGBA &colour);
        uint16_t _length;
        uint16_t _width;
    };
} // Shape

#endif //RENDERER_RECTANGLE_H
