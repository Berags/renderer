//
// Created by jacopo on 11/1/25.
//

#include <utility>

#include "../include/Image.h"

#include <iostream>

#include "stb_image_write.h"

/**
 * Constructor: Initialize an Image object with specified dimensions and filename.
 *
 * @param w Image width in pixels
 * @param h Image height in pixels
 * @param filename Output filename for saving the image
 */
Image::Image(const uint16_t w, const uint16_t h, std::string filename) : width(w), height(h),
                                                                         filename(std::move(filename)) {
    // Validate dimensions to prevent creating invalid or zero-sized images
    if (w <= 0 || h <= 0) {
        throw std::runtime_error("Invalid Image dimensions.");
    }

    // Allocate pixel buffer as a 1D array (flattened 2D array) and initialize to transparent black (0,0,0,0)
    // This is more cache-friendly than a 2D vector and simplifies memory management
    pixel_buffer.resize(static_cast<size_t>(width) * height, {0, 0, 0, 0});
}

/**
 * Set the color of a pixel at the specified coordinates.
 *
 * @param x X-coordinate (column) of the pixel
 * @param y Y-coordinate (row) of the pixel
 * @param colour RGBA color value to set
 *
 */
void Image::set_pixel(const uint16_t x, const uint16_t y, const ColourRGBA8 colour) {
    // Convert 2D coordinates (x, y) to 1D array index using row-major order
    pixel_buffer[static_cast<size_t>(y) * width + x] = colour;
}

/**
 * Save the image to disk as a PNG file.
 *
 * @return true if the image was saved successfully, false otherwise
 */
bool Image::save() const {
    constexpr uint8_t channels = 4; // RGBA (4 channels: Red, Green, Blue, Alpha)

    // Cast pixel buffer to void* for stb_image_write compatibility
    // The const_cast is safe here because stbi_write_png doesn't modify the data
    const auto data = static_cast<void *>(const_cast<unsigned char *>(
        reinterpret_cast<const unsigned char *>(pixel_buffer.data())
    ));

    // Stride is the number of bytes per row, needed for proper PNG encoding
    const uint16_t stride_in_bytes = width * channels;

    // Use stb_image_write library to write the PNG file
    return stbi_write_png(
        filename.c_str(),
        width,
        height,
        channels,
        data,
        stride_in_bytes
    );
}
