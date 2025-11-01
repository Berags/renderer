//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_VECTORIZEDRENDERER_H
#define RENDERER_VECTORIZEDRENDERER_H
#include "Renderer.h"


class VectorizedRenderer : public Strategy {
public:
    void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const override;
};


#endif //RENDERER_VECTORIZEDRENDERER_H
