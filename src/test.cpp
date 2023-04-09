#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "logic.hpp"

image create_random_image(uint32_t width, uint32_t height) {
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
}

void test_encrypt_bit() {
    {
        byte channel = 0;
        byte channel_copy = channel;
        Private::encrypt_bit(channel, 1);
        assert(channel != channel_copy);
    }
    {
        byte channel = 255;
        byte channel_copy = channel;
        Private::encrypt_bit(channel, 1);
        assert(channel != channel_copy);
    }
    {
        byte channel = 0;
        byte channel_copy = channel;
        Private::encrypt_bit(channel, 0);
        assert(channel == channel_copy);
    }
    {
        byte channel = 255;
        byte channel_copy = channel;
        Private::encrypt_bit(channel, 0);
        assert(channel == channel_copy);
    }
    {
        byte channel = 100;
        byte channel_copy = channel;
        Private::encrypt_bit(channel, 1);
        assert(channel != channel_copy);
    }
    {
        byte channel = 100;
        byte channel_copy = channel;
        Private::encrypt_bit(channel, 0);
        assert(channel == channel_copy);
    }
}

int main() {
    test_encryption_decryption();
    std::cout << "encryption/decryption test passed"
              << std::endl;

    test_encrypt_bit();
    std::cout << "encrypt bit test passed" << std::endl;
}
