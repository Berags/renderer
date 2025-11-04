//
// Created by jacopo on 11/1/25.
//

#include "../../include/shape/Rectangle.h"

namespace Shape {
    Rectangle::Rectangle(const uint16_t x, const uint16_t y, const uint8_t z, const uint16_t length,
                         const uint16_t width, const ColourRGBA &colour) : _length(length), _width(width) {
        this->_x = x;
        this->_y = y;
        this->_z = z;
        this->_colour = colour;
    }

    bool Rectangle::isInside(const float px, const float py) const {
        const auto half_len = static_cast<float>(this->getLength()) / 2.0f;
        const auto half_wid = static_cast<float>(this->getWidth()) / 2.0f;
        const auto rect_x = static_cast<float>(this->getX());
        const auto rect_y = static_cast<float>(this->getY());

        return (px >= rect_x - half_len && px <= rect_x + half_len &&
                py >= rect_y - half_wid && py <= rect_y + half_wid);
    }

    Rectangle::Builder &Rectangle::Builder::x(const uint16_t x) {
        _x = x;
        return *this;
    }

    Rectangle::Builder &Rectangle::Builder::y(const uint16_t y) {
        _y = y;
        return *this;
    }

    Rectangle::Builder &Rectangle::Builder::z(const uint8_t z) {
        _z = z;
        return *this;
    }

    Rectangle::Builder &Rectangle::Builder::length(const uint16_t length) {
        _length = length;
        return *this;
    }

    Rectangle::Builder &Rectangle::Builder::width(const uint16_t width) {
        _width = width;
        return *this;
    }

    Rectangle::Builder &Rectangle::Builder::colour(const ColourRGBA &colour) {
        _colour = colour;
        return *this;
    }

    std::unique_ptr<Rectangle> Rectangle::Builder::build() const {
        return std::make_unique<Rectangle>(_x, _y, _z, _length, _width, _colour);
    }
} // Shape
