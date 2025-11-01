//
// Created by jacopo on 10/29/25.
//

#ifndef RENDERER_ISHAPE_H
#define RENDERER_ISHAPE_H
#include <cstdint>

namespace Shape {
    typedef struct ColourRGBA {
        // r, g, b: 0-255
        std::uint8_t r, g, b;

        // a: 0.0-1.0
        // in this case we use float for simplicity,
        // but the renderer will return a RGBA8
        float a;
    } ColourRGBA;

    class IShape {
    public:
        virtual ~IShape() = default;

    protected:
        // Max supported resolution 65535x65535
        // However, it will be limited to 2048x2048
        uint16_t x = 0;
        uint16_t y = 0;

        // Max supported depth 255
        uint8_t z = 0;

        ColourRGBA colour{};
    };
}
#endif //RENDERER_ISHAPE_H
