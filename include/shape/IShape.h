//
// Created by jacopo on 10/29/25.
//

#ifndef RENDERER_ISHAPE_H
#define RENDERER_ISHAPE_H
#include <cstdint>

#include "IShapeVisitor.h"

namespace Shape {
typedef struct alignas(16) ColourRGBA {
  // in this case we use float so that the blend function
  // over thousands of shapes returns a more precise result
  // r, g, b, a: 0.0-1.0
  float r, g, b, a;
} ColourRGBA;

class IShape {
 public:
  virtual ~IShape() = default;

  [[nodiscard]] uint16_t get_x() const { return x_; }

  [[nodiscard]] uint16_t get_y() const { return y_; }

  [[nodiscard]] uint8_t get_z() const { return z_; }

  [[nodiscard]] ColourRGBA get_colour() const { return colour_; }

  virtual void accept(IShapeVisitor &visitor) const = 0;

  [[nodiscard]] virtual bool is_inside(float px, float py) const = 0;

 protected:
  // Max supported resolution 65535x65535
  // However, it will be limited to 2048x2048
  uint16_t x_ = 0;
  uint16_t y_ = 0;

  // Max supported depth 255
  uint8_t z_ = 0;

  ColourRGBA colour_{};
};
}  // namespace Shape
#endif  // RENDERER_ISHAPE_H
