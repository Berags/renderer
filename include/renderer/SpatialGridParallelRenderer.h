//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_SPATIALGRIDPARALLELRENDERER_H
#define RENDERER_SPATIALGRIDPARALLELRENDERER_H
#include "OptimizedParallelRenderer.h"
#include "Renderer.h"

namespace Renderer {
    class SpatialGridParallelRenderer : public OptimizedParallelRenderer {
    public:
        void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const override;

    protected:
        [[nodiscard]] static bool compareZ(const RenderItem *a, const RenderItem *b) {
            if (a->z != b->z) {
                return a->z < b->z;
            }
            return a < b;
        }
    };
}

#endif //RENDERER_SPATIALGRIDPARALLELRENDERER_H
