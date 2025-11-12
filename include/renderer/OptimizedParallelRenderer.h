//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_OPTIMIZEDPARALLELRENDERER_H
#define RENDERER_OPTIMIZEDPARALLELRENDERER_H
#include "Renderer.h"

namespace Renderer {
class OptimizedParallelRenderer : public Strategy {
 public:
  void render(Image &image,
              const std::vector<std::unique_ptr<Shape::IShape> > &shapes)
      const override;

 protected:
  struct alignas(16) RenderItem {
    uint8_t z;

    enum ShapeType { kCircle, kRectangle } type;

    // Unique identifier for stable sorting
    size_t id;

    Shape::ColourRGBA colour;
    float p1, p2, p3, p4;  // shape-specific parameters
  };

  [[nodiscard]] static bool compare_z(const RenderItem &a,
                                      const RenderItem &b) {
    if (a.z != b.z) {
      return a.z < b.z;
    }
    return a.id < b.id;
  }

  class RenderItemVisitor : public Shape::IShapeVisitor {
   public:
    explicit RenderItemVisitor(RenderItem &item);

    void visit(const Shape::Circle &c) override;

    void visit(const Shape::Rectangle &r) override;

   private:
    RenderItem &item_;
  };
};
}  // namespace Renderer

#endif  // RENDERER_OPTIMIZEDPARALLELRENDERER_H
