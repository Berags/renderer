//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_PARALLELRENDERER_H
#define RENDERER_PARALLELRENDERER_H

#include "Renderer.h"

class ParallelRenderer : public Strategy {
public:
    void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const override {
        std::cout << "ParallelRenderer: Rendering in parallel.\n";
    }
};


#endif //RENDERER_PARALLELRENDERER_H
