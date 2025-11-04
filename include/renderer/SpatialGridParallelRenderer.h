//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_SPATIALGRIDPARALLELRENDERER_H
#define RENDERER_SPATIALGRIDPARALLELRENDERER_H
#include "Renderer.h"

namespace Renderer {
    class SpatialGridParallelRenderer : public Strategy {
    public:
        void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const override;

    private:
        struct RenderItem {
            uint8_t z;

            enum ShapeType {
                CIRCLE,
                RECTANGLE
            } type;

            Shape::ColourRGBA colour;
            float p1, p2, p3, p4; // shape-specific parameters
        };

        static bool compareRenderItemZ(const RenderItem *a, const RenderItem *b) {
            if (a->z != b->z) {
                return a->z < b->z;
            }
            return a < b;
        }
    };
}

#endif //RENDERER_SPATIALGRIDPARALLELRENDERER_H
