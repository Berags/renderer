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

            Builder &x(uint16_t x);

            Builder &y(uint16_t y);

            Builder &z(uint8_t z);

            Builder &radius(uint16_t radius);

            Builder &colour(const ColourRGBA &colour);

            [[nodiscard]] std::unique_ptr<Circle> build() const;

        private:
            uint16_t _x{0}, _y{0};
            uint8_t _z{0};
            uint16_t _radius{0};
            ColourRGBA _colour{};
        };

        explicit Circle(uint16_t x, uint16_t y, uint8_t z, uint16_t radius, const ColourRGBA &colour);

        [[nodiscard]] bool isInside(const float px, const float py) const override;

        [[nodiscard]] uint16_t getRadius() const {
            return _radius;
        }

    private:
        uint16_t _radius;
    };
} // Shape

#endif //RENDERER_CIRCLE_H
