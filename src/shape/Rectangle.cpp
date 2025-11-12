//
// Created by jacopo on 11/1/25.
//

#include "../../include/shape/Rectangle.h"

namespace Shape {
Rectangle::Rectangle(const uint16_t x, const uint16_t y, const uint8_t z,
                     const uint16_t length, const uint16_t width,
                     const ColourRGBA &colour)
    : length_(length), width_(width) {
  this->x_ = x;
  this->y_ = y;
  this->z_ = z;
  this->colour_ = colour;
}

bool Rectangle::is_inside(const float px, const float py) const {
  const auto half_len = static_cast<float>(this->get_length()) / 2.0f;
  const auto half_wid = static_cast<float>(this->get_width()) / 2.0f;
  const auto rect_x = static_cast<float>(this->get_x());
  const auto rect_y = static_cast<float>(this->get_y());

  return (px >= rect_x - half_len && px <= rect_x + half_len &&
          py >= rect_y - half_wid && py <= rect_y + half_wid);
}

Rectangle::Builder &Rectangle::Builder::x(const uint16_t x) {
  x_ = x;
  return *this;
}

Rectangle::Builder &Rectangle::Builder::y(const uint16_t y) {
  y_ = y;
  return *this;
}

Rectangle::Builder &Rectangle::Builder::z(const uint8_t z) {
  z_ = z;
  return *this;
}

Rectangle::Builder &Rectangle::Builder::length(const uint16_t length) {
  length_ = length;
  return *this;
}

Rectangle::Builder &Rectangle::Builder::width(const uint16_t width) {
  width_ = width;
  return *this;
}

Rectangle::Builder &Rectangle::Builder::colour(const ColourRGBA &colour) {
  colour_ = colour;
  return *this;
}

std::unique_ptr<Rectangle> Rectangle::Builder::build() const {
  return std::make_unique<Rectangle>(x_, y_, z_, length_, width_, colour_);
}
}  // namespace Shape
