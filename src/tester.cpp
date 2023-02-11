/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include <iostream>

#include "../lib/lodepng/lodepng.h" 

/*
3 ways to decode a PNG from a file to RGBA pixel data (and 2 in-memory ways).
*/

// g++ lodepng.cpp example_decode.cpp -ansi -pedantic -Wall -Wextra -O3

// Example 1
// Decode from disk to raw pixels with a single function call
void decodeOneStep(const char* filename) {
    std::vector<unsigned char> image;  // the raw pixels
    unsigned                   width, height;

    // decode
    unsigned error = lodepng::decode(image, width, height, filename);

    // if there's an error, display it
    if (error)
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    unsigned to_test = 8;
    for (size_t i = 0; i < to_test; i++) {
        for (size_t j = 0; j < 8; j++) {
            unsigned pos = i * 8 + j;
            std::cout << (int)image[pos] << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    const char* filename = argc > 1 ? argv[1] : "test.png";

    decodeOneStep(filename);
}
