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
        typedef struct TileRenderDataSoA {
            std::vector<float> circlesX;
            std::vector<float> circlesY;
            std::vector<float> circlesRadiusSquared;
            std::vector<Shape::ColourRGBA> circlesColour;
            std::vector<size_t> circlesId;
            std::vector<uint8_t> circlesZ;

            std::vector<float> rectanglesXMin;
            std::vector<float> rectanglesYMin;
            std::vector<float> rectanglesXMax;
            std::vector<float> rectanglesYMax;
            std::vector<Shape::ColourRGBA> rectanglesColour;
            std::vector<size_t> rectanglesId;
            std::vector<uint8_t> rectanglesZ;
        } TileDataSoA;

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

        static void mergeColours(TileRenderDataSoA &tileDataSoA, std::vector<bool> &circlesInsidePixelMask, std::vector<bool> &rectanglesInsidePixelMask, std::vector<Shape::ColourRGBA> &shapesInPixelColours) ;
    };
}

#endif //RENDERER_SPATIALGRIDPARALLELRENDERER_H
