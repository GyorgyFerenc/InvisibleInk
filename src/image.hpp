#include <cstdint>
#include <vector>

#include "../lib/lodepng/lodepng.h"
#include "error.hpp"

using byte = unsigned char;

// an RGBA color with 8 bit depth in each channel
struct rgba {
    byte red;
    byte green;
    byte blue;
    byte alpha;
};

// An image in memory
struct image {
    uint32_t          width;
    uint32_t          height;
    std::vector<rgba> pixels;
};

// loads the png from the file and returns it
image load_png(std::string file) {
    // raw image in memory, RGBARGBA...
    std::vector<byte> flat_image;
    uint32_t          width, height;

    // decode
    uint32_t error =
        lodepng::decode(flat_image, width, height, file);

    // if there's an error, display it
    if (error) {
        throw png_load_error{file,
                             lodepng_error_text(error)};
    }

    image image_to_return;
    image_to_return.width = width;
    image_to_return.height = height;

    for (size_t i = 0; i < flat_image.size(); i += 4) {
        auto current_pos = flat_image.begin() + i;

        rgba pixel;
        pixel.red = *(current_pos++);
        pixel.green = *(current_pos++);
        pixel.blue = *(current_pos++);
        pixel.alpha = *(current_pos);

        image_to_return.pixels.push_back(pixel);
    }
    return image_to_return;
}

// saves the png to the file given
void save_png(const image& png, std::string file) {
    std::vector<byte> flat_image;  // the raw pixels
    for (auto&& pixel : png.pixels) {
        flat_image.push_back(pixel.red);
        flat_image.push_back(pixel.green);
        flat_image.push_back(pixel.blue);
        flat_image.push_back(pixel.alpha);
    }

    // Encode the image
    unsigned error = lodepng::encode(
        file, flat_image, png.width, png.height);

    // if there's an error, display it
    if (error) {
        throw png_save_error{file,
                             lodepng_error_text(error)};
    }
}