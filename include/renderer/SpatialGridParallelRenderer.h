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
        void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const override;

    protected:
        [[nodiscard]] static bool compareZ(const RenderItem *a, const RenderItem *b) {
            if (a->z != b->z) {
                return a->z < b->z;
            }
            return a->id < b->id;
        }

    private:
        class RenderItemVisitor : public Shape::IShapeVisitor {
        public:
            explicit RenderItemVisitor(RenderItem &item);

            void visit(const Shape::Circle &c) override;

            void visit(const Shape::Rectangle &r) override;

        private:
            RenderItem &_item;
        };
    };
}

#endif //RENDERER_SPATIALGRIDPARALLELRENDERER_H
