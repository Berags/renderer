//
// Created by jacopo on 10/29/25.
//

#ifndef RENDERER_CIRCLE_H
#define RENDERER_CIRCLE_H

#include "IShape.h"
#include <memory>

namespace Shape {
    class Circle : public IShape {
    public:
        class Builder {
        public:
            Builder() = default;

            Builder &x(int16_t x);
            Builder &y(int16_t y);
            Builder &z(int16_t z);
            Builder &radius(int16_t radius);
            Builder &colour(const ColourRGBA &colour);

            [[nodiscard]] Circle build() const;

        private:
            int16_t _x{0}, _y{0}, _z{0};
            int16_t _radius{0};
            ColourRGBA _colour{};
        };

    private:
        explicit Circle(int16_t x, int16_t y, int16_t z, int16_t radius, const ColourRGBA &colour);
        int16_t _radius;
    };
} // Shape

#endif //RENDERER_CIRCLE_H