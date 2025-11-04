//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_VECTORIZEDRENDERER_H
#define RENDERER_VECTORIZEDRENDERER_H
#include "Renderer.h"

class OptimizedParallelRenderer : public Strategy {
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

    static bool compareRenderItemZ(const RenderItem &a, const RenderItem &b) {
        return a.z < b.z;
    }
};


#endif //RENDERER_VECTORIZEDRENDERER_H
