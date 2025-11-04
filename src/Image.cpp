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
Image::Image(const uint16_t w, const uint16_t h, std::string filename) : _width(w), _height(h),
                                                                         _filename(std::move(filename)) {
    // Validate dimensions to prevent creating invalid or zero-sized images
    if (w == 0 || h == 0) {
        throw std::runtime_error("Invalid Image dimensions.");
    }

    if (w > MAX_WIDTH || h > MAX_HEIGHT) {
        throw std::runtime_error("Image dimensions too large.");
    }

    // Allocate pixel buffer as a 1D array (flattened 2D array) and initialize to transparent black (0,0,0,0)
    // This is more cache-friendly than a 2D vector and simplifies memory management
    _pixelBuffer.resize(static_cast<size_t>(_width) * _height, {0, 0, 0, 0});
}

/**
 * Save the image to disk as a PNG file.
 *
 * @return true if the image was saved successfully, false otherwise
 */
bool Image::save() const {
    constexpr uint8_t channels = 4; // RGBA (4 channels: Red, Green, Blue, Alpha)

    // Cast pixel buffer to void* for stb_image_write compatibility
    const void *data = _pixelBuffer.data();

    // Stride is the number of bytes per row, needed for proper PNG encoding
    const uint16_t stride_in_bytes = _width * channels;

    // Use stb_image_write library to write the PNG file
    return stbi_write_png(
        _filename.c_str(),
        _width,
        _height,
        channels,
        data,
        stride_in_bytes
    );
}
