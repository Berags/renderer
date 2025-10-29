//
// Created by jacopo on 10/29/25.
//

#ifndef RENDERER_ISHAPE_H
#define RENDERER_ISHAPE_H

namespace Shape {
    typedef struct ColourRGBA {
        float r, g, b, a;
    } ColourRGBA;

    class IShape {
    public:
        virtual ~IShape() = default;

    protected:
        float x = 0;
        float y = 0;
        float z = 0;

        ColourRGBA colour{};
    };
}
#endif //RENDERER_ISHAPE_H
