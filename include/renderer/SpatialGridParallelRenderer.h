//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_SPATIALGRIDPARALLELRENDERER_H
#define RENDERER_SPATIALGRIDPARALLELRENDERER_H
#include "OptimizedParallelRenderer.h"
#include "Renderer.h"
#include "shape/Circle.h"
#include "shape/Rectangle.h"

namespace Renderer {
class SpatialGridParallelRenderer : public OptimizedParallelRenderer {
 public:
  void render(Image &image,
              const std::vector<std::unique_ptr<Shape::IShape> > &shapes)
      const override;

 protected:
  struct TileRenderDataSoA {
    std::vector<float> circles_x;
    std::vector<float> circles_y;
    std::vector<float> circles_radius_squared;
    std::vector<Shape::ColourRGBA> circles_colour;
    std::vector<size_t> circles_id;
    std::vector<uint8_t> circles_z;

    std::vector<float> rectangles_x_min;
    std::vector<float> rectangles_y_min;
    std::vector<float> rectangles_x_max;
    std::vector<float> rectangles_y_max;
    std::vector<Shape::ColourRGBA> rectangles_colour;
    std::vector<size_t> rectangles_id;
    std::vector<uint8_t> rectangles_z;
  } TileDataSoA;

  [[nodiscard]] static bool compare_z(const RenderItem *a,
                                      const RenderItem *b) {
    if (a->z != b->z) {
      return a->z < b->z;
    }
    return a->id < b->id;
  }

 private:

  static void merge_colours(
      TileRenderDataSoA &tile_data,
      std::vector<bool> &circles_inside_pixel_mask,
      std::vector<bool> &rectangles_inside_pixel_mask,
      std::vector<Shape::ColourRGBA> &shapes_in_pixel_colours);
};
}  // namespace Renderer

#endif  // RENDERER_SPATIALGRIDPARALLELRENDERER_H
