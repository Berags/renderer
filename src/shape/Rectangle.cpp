//
// Created by jacopo on 11/1/25.
//

#include "../../include/shape/Rectangle.h"

namespace Shape {
    Rectangle::Rectangle(const uint16_t x, const uint16_t y, const uint8_t z, const uint16_t length, const uint16_t width, const ColourRGBA &colour) : _length(length), _width(width) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->colour = colour;
    }

    Rectangle::Builder& Rectangle::Builder::x(const uint16_t x) {
        _x = x;
        return *this;
    }

    Rectangle::Builder& Rectangle::Builder::y(const uint16_t y) {
        _y = y;
        return *this;
    }

    Rectangle::Builder& Rectangle::Builder::z(const uint8_t z) {
        _z = z;
        return *this;
    }

    Rectangle::Builder& Rectangle::Builder::length(const uint16_t length) {
        _length = length;
        return *this;
    }

    Rectangle::Builder& Rectangle::Builder::width(const uint16_t width) {
        _width = width;
        return *this;
    }

    Rectangle::Builder& Rectangle::Builder::colour(const ColourRGBA &colour) {
        _colour = colour;
        return *this;
    }

    Rectangle Rectangle::Builder::build() const {
        return Rectangle(_x, _y, _z, _length, _width, _colour);
    }
} // Shape