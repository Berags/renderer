//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_OPTIMIZEDPARALLELRENDERER_H
#define RENDERER_OPTIMIZEDPARALLELRENDERER_H
#include "Renderer.h"

namespace Renderer {
    class OptimizedParallelRenderer : public Strategy {
    public:
        void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const override;

    protected:
        struct alignas(16) RenderItem {
            uint8_t z;

            enum ShapeType {
                CIRCLE,
                RECTANGLE
            } type;

            // Unique identifier for stable sorting
            size_t id;

            Shape::ColourRGBA colour;
            float p1, p2, p3, p4; // shape-specific parameters
        };

        [[nodiscard]] static bool compareZ(const RenderItem &a, const RenderItem &b) {
            if (a.z != b.z) {
                return a.z < b.z;
            }
            return a.id < b.id;
        }
    };
}

#endif //RENDERER_OPTIMIZEDPARALLELRENDERER_H
