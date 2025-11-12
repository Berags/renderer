//
// Created by jacopo on 10/29/25.
//

#ifndef RENDERER_CIRCLE_H
#define RENDERER_CIRCLE_H

#include <memory>

#include "IShape.h"

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
    uint16_t x_{0}, y_{0};
    uint8_t z_{0};
    uint16_t radius_{0};
    ColourRGBA colour_{};
  };

  explicit Circle(uint16_t x, uint16_t y, uint8_t z, uint16_t radius,
                  const ColourRGBA &colour);

  [[nodiscard]] bool is_inside(const float px, const float py) const override;

  [[nodiscard]] uint16_t get_radius() const { return radius_; }

  void accept(IShapeVisitor &visitor) const override { visitor.visit(*this); }

 private:
  uint16_t radius_;
};
}  // namespace Shape

#endif  // RENDERER_CIRCLE_H
