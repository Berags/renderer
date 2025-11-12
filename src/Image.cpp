//
// Created by jacopo on 11/1/25.
//

#include "../include/Image.h"

#include <utility>

#include "absl/log/log.h"
#include "stb_image_write.h"

/**
 * Constructor: Initialize an Image object with specified dimensions and
 * filename.
 *
 * @param w Image width in pixels
 * @param h Image height in pixels
 * @param filename Output filename for saving the image
 */
std::unique_ptr<Image> Image::Create(const uint16_t w, const uint16_t h,
                                     std::string filename) {
  // Validate dimensions to prevent creating invalid or zero-sized images
  if (w == 0 || h == 0) {
    LOG(ERROR) << "Invalid Image dimensions (" << w << "x" << h << ").";
    return nullptr;
  }

  if (w > kMaxWidth || h > kMaxHeight) {
    LOG(ERROR) << "Invalid Image dimensions. Max allowed: " << kMaxWidth << "x"
               << kMaxHeight;
    return nullptr;
  }
  return std::unique_ptr<Image>(new Image(w, h, std::move(filename)));
}

/**
 * Save the image to disk as a PNG file.
 *
 * @return true if the image was saved successfully, false otherwise
 */
bool Image::save() const {
  constexpr uint8_t kChannels =
      4;  // RGBA (4 channels: Red, Green, Blue, Alpha)

  // Cast pixel buffer to void* for stb_image_write compatibility
  const void* data = pixel_buffer_.data();

  // Stride is the number of bytes per row, needed for proper PNG encoding
  const uint16_t stride_in_bytes = width_ * kChannels;

  // Use stb_image_write library to write the PNG file
  return stbi_write_png(filename_.c_str(), width_, height_, kChannels, data,
                        stride_in_bytes);
}

Image::Image(const uint16_t w, const uint16_t h, std::string filename)
    : width_(w), height_(h), filename_(std::move(filename)) {
  // Allocate pixel buffer as a 1D array (flattened 2D array) and initialize to
  // transparent black (0,0,0,0) This is more cache-friendly than a 2D vector
  // and simplifies memory management
  pixel_buffer_.resize(static_cast<size_t>(width_) * height_, {0, 0, 0, 0});
}