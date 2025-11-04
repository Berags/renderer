//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <cmath>
#include <memory>

#include "../Image.h"
#include "../shape/IShape.h"

namespace Renderer {
    typedef enum RenderStrategy {
        SEQUENTIAL,
        SIMPLE_PARALLEL,
        OPTIMIZED_PARALLEL,
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

        void setStrategy(RenderStrategy strategy);

        void render(Image &image, const std::vector<std::unique_ptr<Shape::IShape> > &shapes) const {
            if (!_strategy) {
                throw std::runtime_error("Rendering strategy is not set.");
            }

            _strategy->render(image, shapes);
        }

        // Sequential Alpha blending function
        // https://en.wikipedia.org/wiki/Alpha_compositing#Description
        [[nodiscard]] static Shape::ColourRGBA blend(const Shape::ColourRGBA &old_colour,
                                                     const Shape::ColourRGBA &new_colour) {
            const float new_alpha = new_colour.a;
            const float inv_alpha = 1.0f - new_alpha;

            return {
                new_colour.r * new_alpha + old_colour.r * inv_alpha,
                new_colour.g * new_alpha + old_colour.g * inv_alpha,
                new_colour.b * new_alpha + old_colour.b * inv_alpha,
                new_alpha + old_colour.a * inv_alpha
            };
        }

        [[nodiscard]] static ColourRGBA8 convertToRGBA8(const Shape::ColourRGBA &float_colour) {
            return {
                static_cast<std::uint8_t>(std::lround(float_colour.r * 255.0f)),
                static_cast<std::uint8_t>(std::lround(float_colour.g * 255.0f)),
                static_cast<std::uint8_t>(std::lround(float_colour.b * 255.0f)),
                static_cast<std::uint8_t>(std::lround(float_colour.a * 255.0f))
            };
        }

    private:
        std::unique_ptr<Strategy> _strategy;
    };
}

#endif //RENDERER_RENDERER_H
