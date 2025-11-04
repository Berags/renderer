//
// Created by jacopo on 10/29/25.
//

#include "../../include/shape/Circle.h"

namespace Shape {
    Circle::Circle(const uint16_t x, const uint16_t y, const uint8_t z, const uint16_t radius,
                   const ColourRGBA &colour) : _radius(radius) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->colour = colour;
    }

    Circle::Builder &Circle::Builder::x(const uint16_t x) {
        _x = x;
        return *this;
    }

    Circle::Builder &Circle::Builder::y(const uint16_t y) {
        _y = y;
        return *this;
    }

    Circle::Builder &Circle::Builder::z(const uint8_t z) {
        _z = z;
        return *this;
    }

    Circle::Builder &Circle::Builder::radius(const uint16_t radius) {
        _radius = radius;
        return *this;
    }

    Circle::Builder &Circle::Builder::colour(const ColourRGBA &colour) {
        _colour = colour;
        return *this;
    }

    std::unique_ptr<Circle> Circle::Builder::build() const {
        return std::make_unique<Circle>(_x, _y, _z, _radius, _colour);
    }

    bool Circle::isInside(const float px, const float py) const {
        const float dx = static_cast<float>(this->getX()) - px;
        const float dy = static_cast<float>(this->getY()) - py;
        return (dx * dx + dy * dy) <= static_cast<float>(this->getRadius() * this->getRadius());
    }
} // Shape
