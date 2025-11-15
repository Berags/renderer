//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <cmath>
#include <memory>

#include "../Image.h"
#include "../shape/IShape.h"
#include "absl/log/log.h"

namespace Renderer {
typedef enum RenderStrategy {
  kSequential,
  kSimpleParallel,
  kOptimizedParallel,
  kSpatialGridParallel
} RenderStrategy;

class Strategy {
 public:
  virtual ~Strategy() = default;

  virtual void render(
      Image &image,
      const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const = 0;
};

class Renderer {
 public:
  Renderer() = default;

  ~Renderer() = default;

  void set_strategy(RenderStrategy strategy);

  void render(
      Image &image,
      const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
    if (!strategy_) {
      LOG(ERROR) << "Renderer: Rendering strategy is not set.";
      return;
    }

    strategy_->render(image, shapes);
  }

  // Sequential Alpha blending function
  // https://en.wikipedia.org/wiki/Alpha_compositing#Description
  [[nodiscard]] static Shape::ColourRGBA blend(
      const Shape::ColourRGBA &old_colour,
      const Shape::ColourRGBA &new_colour) {
    const float new_alpha = new_colour.a;
    const float inv_alpha = 1.0f - new_alpha;

    return {new_colour.r + old_colour.r * inv_alpha,
            new_colour.g + old_colour.g * inv_alpha,
            new_colour.b + old_colour.b * inv_alpha,
            new_alpha + old_colour.a * inv_alpha};
  }

  [[nodiscard]] static ColourRGBA8 convert_to_rgba8(
      const Shape::ColourRGBA &float_colour) {
    const auto [r, g, b, a] = unpremultiply(float_colour);
    return {static_cast<std::uint8_t>(std::lround(r * 255.0f)),
            static_cast<std::uint8_t>(std::lround(g * 255.0f)),
            static_cast<std::uint8_t>(std::lround(b * 255.0f)),
            static_cast<std::uint8_t>(std::lround(a * 255.0f))};
  }

  [[nodiscard]] static Shape::ColourRGBA unpremultiply(
      const Shape::ColourRGBA &colour) {
    if (std::fabs(colour.r) < std::numeric_limits<float>::epsilon()) {
      return {0.f, 0.f, 0.f, 0.f};
    }
    const float inv_alpha = 1.0f / colour.a;
    return {colour.r * inv_alpha, colour.g * inv_alpha, colour.b * inv_alpha,
            colour.a};
  }

 private:
  std::unique_ptr<Strategy> strategy_;
};
}  // namespace Renderer

#endif  // RENDERER_RENDERER_H
