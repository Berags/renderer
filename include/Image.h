//
// Created by jacopo on 11/1/25.
//

#ifndef RENDERER_IMAGE_H
#define RENDERER_IMAGE_H
#include <cstdint>
#include <stdexcept>
#include <vector>

/**
 * RGBA color representation using 8-bit per channel (0-255).
 * Used for rendering semi-transparent shapes with alpha blending.
 */
typedef struct ColourRGBA8 {
    uint8_t r;  // Red channel (0-255)
    uint8_t g;  // Green channel (0-255)
    uint8_t b;  // Blue channel (0-255)
    uint8_t a;  // Alpha/transparency channel (0=transparent, 255=opaque)
} ColourRGBA8;

/**
 * Image buffer for rendering and saving pixel data.
 *
 * This class manages a 2D pixel buffer and provides functionality to:
 * - Set individual pixels (thread-safe when accessing different pixels)
 * - Save the buffer to a PNG file using stb_image_write
 */
class Image {
public:
    /**
     * Construct an image with specified dimensions and output filename.
     *
     * @param w Image width in pixels (must be > 0)
     * @param h Image height in pixels (must be > 0)
     * @param filename Output PNG filename for saving
     * @throws std::runtime_error if dimensions are invalid
     */
    Image(uint16_t w, uint16_t h, std::string filename);

    /**
     * Set a pixel color at specified coordinates.
     *
     * @param x X coordinate (0 to width-1)
     * @param y Y coordinate (0 to height-1)
     * @param colour RGBA color value to set
     */
    inline void set_pixel(uint16_t x, uint16_t y, ColourRGBA8 colour);

    /**
     * Save the image buffer to a PNG file.
     *
     * @return true if save succeeded, false otherwise
     * [[nodiscard]] ensures caller checks the result (important for error handling)
     */
    [[nodiscard]] bool save() const;

private:
    uint16_t width;   // Image width in pixels
    uint16_t height;  // Image height in pixels

    // Flattened 1D buffer: index = y * width + x
    std::vector<ColourRGBA8> pixel_buffer;

    std::string filename;  // Output PNG filename
};


#endif //RENDERER_IMAGE_H
