//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_SIMPLEPARALLELRENDERER_H
#define RENDERER_SIMPLEPARALLELRENDERER_H

#include "Renderer.h"

namespace Renderer {
class SimpleParallelRenderer : public Strategy {
 public:
  void render(Image &image,
              const std::vector<std::unique_ptr<Shape::IShape> > &shapes)
      const override;
};
}  // namespace Renderer

#endif  // RENDERER_SIMPLEPARALLELRENDERER_H
