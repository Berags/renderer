//
// Created by jacopo on 11/2/25.
//

#ifndef RENDERER_TRIANGLE_H
#define RENDERER_TRIANGLE_H
#include <memory>

#include "IShape.h"

namespace Shape {
    class Triangle : public IShape {
    public:
        class Builder {
        public:
            Builder() = default;

            Builder &p1(const Point2D &p1);

            Builder &p2(const Point2D &p2);

            Builder &p3(const Point2D &p3);

            Builder &z(uint8_t z);

            Builder &colour(const ColourRGBA &colour);

            [[nodiscard]] std::unique_ptr<Triangle> build() const;

        private:
            Point2D _p1{}, _p2{}, _p3{};
            uint8_t _z{0};
            ColourRGBA _colour{};
        };

        explicit Triangle(const Point2D &p1, const Point2D &p2, const Point2D &p3, uint8_t z,
                          const ColourRGBA &colour);

        [[nodiscard]] bool isInside(float px, float py) const override;

    private:
        Point2D _p1, _p2, _p3;
    };
} // Shape

#endif //RENDERER_TRIANGLE_H
