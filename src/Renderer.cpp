//
// Created by jacopo on 10/28/25.
//

#include "../include/Renderer.h"

#include <iostream>
#include <ostream>

#include "../include/SequentialRenderer.h"
#include "../include/ParallelRenderer.h"

void Renderer::set_strategy(const RenderStrategy strategy) {
    switch (strategy) {
        case SEQUENTIAL:
            _strategy = std::make_unique<SequentialRenderer>();
            break;
        case PARALLEL:
            _strategy = std::make_unique<ParallelRenderer>();
            break;
        default:
            std::cout << "Renderer: Unknown strategy type.\n";
            _strategy = nullptr;
            break;
    }
}

// Alpha blending function
// https://en.wikipedia.org/wiki/Alpha_compositing#Description
ColourRGBA8 Renderer::blend(const ColourRGBA8 old_colour, const Shape::ColourRGBA new_colour) {
    const float alpha = new_colour.a;
    const float inv_alpha = 1.0f - alpha;

    return {
        static_cast<uint8_t>(new_colour.r * alpha + old_colour.r * inv_alpha),
        static_cast<uint8_t>(new_colour.g * alpha + old_colour.g * inv_alpha),
        static_cast<uint8_t>(new_colour.b * alpha + old_colour.b * inv_alpha),
        static_cast<uint8_t>(255.0f * alpha + old_colour.a * inv_alpha)
    };
}
