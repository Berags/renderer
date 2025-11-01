//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_IMAGE_H
#define RENDERER_IMAGE_H
#include <cstdint>
#include <stdexcept>
#include <vector>

typedef struct ColourRGBA8 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} ColourRGBA8;

class Image {
public:
    Image(uint16_t w, uint16_t h, std::string filename);

    inline void set_pixel(uint16_t x, uint16_t y, ColourRGBA8 colour);

    [[nodiscard]] bool save() const;

private:
    uint16_t width;
    uint16_t height;
    std::vector<ColourRGBA8> pixel_buffer;

    std::string filename;
};


#endif //RENDERER_IMAGE_H
