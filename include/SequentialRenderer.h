//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_SEQUENTIALRENDERER_H
#define RENDERER_SEQUENTIALRENDERER_H

#include "Renderer.h"

class SequentialRenderer : public Strategy {
public:
    void render() const override;
};

#endif //RENDERER_SEQUENTIALRENDERER_H
