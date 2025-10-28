//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_SEQUENTIALRENDERER_H
#define RENDERER_SEQUENTIALRENDERER_H

#include "Renderer.h"

class SequentialRenderer : public Strategy {
public:
    void render() const override {
        std::cout << "SequentialRenderer: Rendering sequentially.\n";
    }
};

#endif //RENDERER_SEQUENTIALRENDERER_H
