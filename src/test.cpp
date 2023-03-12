#include <cassert>
#include <iostream>
#include <random>

#include "logic.hpp"

image create_random_image(uint width, uint height) {
    image result;
    result.width = width;
    result.height = height;

    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            rgba pixel;
            pixel.red = rand() % 255;
            pixel.blue = rand() % 255;
            pixel.green = rand() % 255;
            pixel.alpha = rand() % 255;

            result.pixels.push_back(pixel);
        }
    }
    return result;
}

void test_encryption_decryption() {
    srand(time(NULL));

    std::vector<byte> byte_array{{1, 2, 3, 4}};
    auto              img = create_random_image(10, 10);
    auto              key = img;  // copy it

    encrypt(img, byte_array);  // using img because encrypt
                               // mutates it
    auto result = decrypt(key, img);

    for (size_t i = 0; i < byte_array.size(); i++) {
        assert(byte_array[i] == result[i]);
    }

    std::cout << "encryption/decryption test passed"<<std::endl;
}

int main() {
    test_encryption_decryption();
}
