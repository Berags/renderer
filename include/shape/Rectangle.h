//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_RECTANGLE_H
#define RENDERER_RECTANGLE_H
#include <memory>

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

    [[nodiscard]] std::unique_ptr<Rectangle> build() const;

   private:
    uint16_t x_{0}, y_{0};
    uint8_t z_{0};
    uint16_t length_{0}, width_{0};
    ColourRGBA colour_{};
  };

  explicit Rectangle(uint16_t x, uint16_t y, uint8_t z, uint16_t length,
                     uint16_t width, const ColourRGBA &colour);

  [[nodiscard]] bool is_inside(float px, float py) const override;

  [[nodiscard]] uint16_t get_length() const { return length_; }

  [[nodiscard]] uint16_t get_width() const { return width_; }

  void accept(IShapeVisitor &visitor) const override { visitor.visit(*this); }

 private:
  uint16_t length_;
  uint16_t width_;
};
}  // namespace Shape

#endif  // RENDERER_RECTANGLE_H
