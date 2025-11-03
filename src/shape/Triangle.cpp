//
// Created by jacopo on 11/2/25.
//

#include "../../include/shape/Triangle.h"

#include "utils/Utils.h"

namespace Shape {
    Triangle::Triangle(const Point2D &p1, const Point2D &p2, const Point2D &p3, const uint8_t z,
                       const ColourRGBA &colour) : _p1(p1), _p2(p2), _p3(p3) {
        this->z = z;
        this->colour = colour;
        this->x = static_cast<uint16_t>((p1.x + p2.x + p3.x) / 3);
        this->y = static_cast<uint16_t>((p1.y + p2.y + p3.y) / 3);
    }

    bool Triangle::isInside(const float px, const float py) const {
        // Using barycentric coordinates to determine if the point is inside the triangle
        const float x0 = _p1.x;
        const float y0 = _p1.y;
        const float x1 = _p2.x;
        const float y1 = _p2.y;
        const float x2 = _p3.x;
        const float y2 = _p3.y;

        const float denom = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
        if (Utils::equal(denom, 0.0f)) {
            return false; // Degenerate triangle
        }
        const float a = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2)) / denom;
        const float b = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) / denom;
        const float c = 1.0f - a - b;

        return (a >= 0) && (b >= 0) && (c >= 0);
    }

    Triangle::Builder &Triangle::Builder::p1(const Point2D &p1) {
        _p1 = p1;
        return *this;
    }

    Triangle::Builder &Triangle::Builder::p2(const Point2D &p2) {
        _p2 = p2;
        return *this;
    }

    Triangle::Builder &Triangle::Builder::p3(const Point2D &p3) {
        _p3 = p3;
        return *this;
    }

    Triangle::Builder &Triangle::Builder::z(const uint8_t z) {
        _z = z;
        return *this;
    }

    Triangle::Builder &Triangle::Builder::colour(const ColourRGBA &colour) {
        _colour = colour;
        return *this;
    }

    std::unique_ptr<Triangle> Triangle::Builder::build() const {
        return std::make_unique<Triangle>(_p1, _p2, _p3, _z, _colour);
    }
} // Shape
