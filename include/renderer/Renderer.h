//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <iostream>
#include <memory>

#include "../Image.h"
#include "../shape/IShape.h"

typedef enum RenderStrategy {
    SEQUENTIAL,
    SIMPLE_PARALLEL
} RenderStrategy;

class Strategy {
public:
    virtual ~Strategy() = default;

    virtual void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const = 0;
};


class Renderer {
public:
    Renderer() = default;

    ~Renderer() = default;

    void set_strategy(RenderStrategy strategy);

    void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
        if (!_strategy) {
            throw std::runtime_error("Rendering strategy is not set.");
        }

        _strategy->render(image, shapes);
    }

    [[nodiscard]] static ColourRGBA8 blend(const ColourRGBA8 &old_colour, const Shape::ColourRGBA &new_colour);

private:
    std::unique_ptr<Strategy> _strategy;
};


#endif //RENDERER_RENDERER_H
