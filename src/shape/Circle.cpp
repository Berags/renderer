//
// Created by jacopo on 10/29/25.
//

#include "../../include/shape/Circle.h"

namespace Shape {
Circle::Circle(const uint16_t x, const uint16_t y, const uint8_t z,
               const uint16_t radius, const ColourRGBA &colour)
    : radius_(radius) {
  this->x_ = x;
  this->y_ = y;
  this->z_ = z;
  this->colour_ = colour;
}

Circle::Builder &Circle::Builder::x(const uint16_t x) {
  x_ = x;
  return *this;
}

Circle::Builder &Circle::Builder::y(const uint16_t y) {
  y_ = y;
  return *this;
}

Circle::Builder &Circle::Builder::z(const uint8_t z) {
  z_ = z;
  return *this;
}

Circle::Builder &Circle::Builder::radius(const uint16_t radius) {
  radius_ = radius;
  return *this;
}

Circle::Builder &Circle::Builder::colour(const ColourRGBA &colour) {
  colour_ = colour;
  return *this;
}

std::unique_ptr<Circle> Circle::Builder::build() const {
  return std::make_unique<Circle>(x_, y_, z_, radius_, colour_);
}

bool Circle::is_inside(const float px, const float py) const {
  const float dx = static_cast<float>(this->get_x()) - px;
  const float dy = static_cast<float>(this->get_y()) - py;
  return (dx * dx + dy * dy) <= static_cast<float>(this->get_radius()) *
                                    static_cast<float>(this->get_radius());
}
}  // namespace Shape
