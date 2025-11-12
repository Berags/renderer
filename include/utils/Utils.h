//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_UTILS_H
#define RENDERER_UTILS_H
#include <memory>
#include <random>
#include <vector>

#include "Image.h"
#include "shape/Circle.h"
#include "shape/IShape.h"
#include "shape/Rectangle.h"

namespace Utils {
/**
 * Creates `n` random shapes (circles or rectangles) and appends them to the
 * provided `shapes` vector.
 *
 * @param shapes Reference to a vector of unique pointers to IShape objects. New
 * shapes will be added to this vector.
 * @param image Reference to an Image object, used to determine the valid ranges
 * for shape positions.
 * @param n The number of shapes to create.
 *
 * Behavior:
 * - For each shape, randomly chooses between a circle and a rectangle (50%
 * chance each).
 * - Position (x, y): Randomly chosen within the image dimensions ([0, width-1],
 * [0, height-1]).
 * - z: Randomly chosen in [0, 254].
 * - Color: Each channel (R, G, B, A) in [0.0, 1.0].
 * - Circle radius: [20, 119].
 * - Rectangle length and width: [20, 169].
 */
inline void create_shapes(std::vector<std::unique_ptr<Shape::IShape> > &shapes,
                          const Image &image, const uint16_t n) {
  // Using a fixed seed for reproducibility
  static std::mt19937 gen(232710);

  const uint16_t width = image.get_width();
  const uint16_t height = image.get_height();

  std::uniform_int_distribution<uint16_t> x_distribution(0, width - 1);
  std::uniform_int_distribution<uint16_t> y_distribution(0, height - 1);
  std::uniform_int_distribution<uint8_t> z_distribution(0, 255);
  std::uniform_real_distribution colour_distribution(0.0f, 1.0f);
  std::uniform_int_distribution type_distribution(0, 3);

  std::uniform_int_distribution<uint16_t> circle_radius_distribution(20, 119);
  std::uniform_int_distribution<uint16_t> rectangle_length_distribution(20,
                                                                        169);
  std::uniform_int_distribution<uint16_t> rectangle_width_distribution(20, 169);

  for (uint16_t i = 0; i < n; ++i) {
    // 75% chance to create a circle
    if (type_distribution(gen) != 0) {
      shapes.push_back(
          Shape::Circle::Builder()
              .x(x_distribution(gen))
              .y(y_distribution(gen))
              .z(z_distribution(gen))
              .radius(circle_radius_distribution(gen))
              .colour({colour_distribution(gen), colour_distribution(gen),
                       colour_distribution(gen), colour_distribution(gen)})
              .build());
    } else {
      shapes.push_back(
          Shape::Rectangle::Builder()
              .x(x_distribution(gen))
              .y(y_distribution(gen))
              .z(z_distribution(gen))
              .length(rectangle_length_distribution(gen))
              .width(rectangle_width_distribution(gen))
              .colour({colour_distribution(gen), colour_distribution(gen),
                       colour_distribution(gen), colour_distribution(gen)})
              .build());
    }
  }
}
}  // namespace Utils

#endif  // RENDERER_UTILS_H
