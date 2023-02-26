#pragma once

#include <cstdint>
#include <fstream>
#include <iterator>

#include "cli.hpp"
#include "image.hpp"
#include "log.hpp"

namespace Private {

// Reads it as byte array
std::vector<byte> read_data_to_be_encrypted(std::string file) {
    std::ifstream input(file, std::ios::binary);

    if (!input.is_open()) {
        error_file_can_not_be_read(file);
    }

    // get length of file
    input.seekg(0, std::ios::end);
    size_t length = input.tellg();
    input.seekg(0, std::ios::beg);

    // Stop eating new lines in binary mode!!!
    input.unsetf(std::ios::skipws);

    std::vector<byte> result;
    result.reserve(length);

    // read the data:
    result.insert(
        result.begin(), std::istream_iterator<byte>(input), std::istream_iterator<byte>());

    return result;
}

// gets the the bit from byte b in the position pos
// bits are numbered from most significant to least
bool get_bit(const byte& b, uint pos) {
    uint mask = 1 << (7 - pos);
    return (b & mask) != 0;
}

// encrypts 1 byte of data into two pixels
void encrypt_into_pixels(rgba& pixel1, rgba& pixel2, byte data) {
    if (pixel1.red < 255)
        pixel1.red += get_bit(data, 0);
    else
        pixel1.red -= get_bit(data, 0);

    if (pixel1.green < 255)
        pixel1.green += get_bit(data, 1);
    else
        pixel1.green -= get_bit(data, 1);

    if (pixel1.blue < 255)
        pixel1.blue += get_bit(data, 2);
    else
        pixel1.blue -= get_bit(data, 2);

    if (pixel1.alpha < 255)
        pixel1.alpha += get_bit(data, 3);
    else
        pixel1.alpha -= get_bit(data, 3);

    if (pixel2.red < 255)
        pixel2.red += get_bit(data, 4);
    else
        pixel2.red -= get_bit(data, 4);

    if (pixel2.green < 255)
        pixel2.green += get_bit(data, 5);
    else
        pixel2.green -= get_bit(data, 5);

    if (pixel2.blue < 255)
        pixel2.blue += get_bit(data, 6);
    else
        pixel2.blue -= get_bit(data, 6);

    if (pixel2.alpha < 255)
        pixel2.alpha += get_bit(data, 7);
    else
        pixel2.alpha -= get_bit(data, 7);
}

// encrypts the length into the first 4 byte, which is the first 8 pixel
void encrypt_length_of_data(image& img, uint32_t lenght) {
    byte first_byte = ((byte*)&lenght)[0];   // Getting the first byte of length
    byte second_byte = ((byte*)&lenght)[1];  // Getting the second byte of length
    byte third_byte = ((byte*)&lenght)[2];   // Getting the third byte of length
    byte fourth_byte = ((byte*)&lenght)[3];  // Getting the fourth byte of length

    // we need to reverse it because of little endian
    std::vector<byte> data{{fourth_byte, third_byte, second_byte, first_byte}};
    uint              i = 0;

    for (auto&& one_byte : data) {
        auto& pixel1 = img.pixels[i];
        auto& pixel2 = img.pixels[i + 1];
        i += 2;

        encrypt_into_pixels(pixel1, pixel2, one_byte);
    }
}

/*
    Do the encryption pipeline.

    Load key png.
    Load data to be encrypted.
    Check for enough space in the photo.
    Encrypt the data into the image.
    Save image.
*/
void encrypt(std::string key_file, std::string input_file, std::string output_file) {
    auto key = load_png(key_file);
    auto data = read_data_to_be_encrypted(input_file);

    uint32_t length_of_data = data.size();

    // Every 2 pixel can hold 1 byte of information
    // First 4 byte is lenght of data
    bool enough_space = length_of_data + 4 <= key.pixels.size() / 2;
    if (!enough_space) {
        error_not_enough_space(length_of_data, key.pixels.size() / 2);
    }

    encrypt_length_of_data(key, length_of_data);

    // First 4 byte is lenght of data
    // aka first 8 pixel
    uint i = 8;
    for (auto&& one_byte : data) {
        auto& pixel1 = key.pixels[i];
        auto& pixel2 = key.pixels[i + 1];
        i += 2;

        encrypt_into_pixels(pixel1, pixel2, one_byte);
    }

    save_png(key, output_file);
}

// Extractes the byte from the pixels.
byte get_byte(rgba& img1_pixel1, rgba& img1_pixel2, rgba& img2_pixel1, rgba& img2_pixel2) {
    bool bit0 = img1_pixel1.red != img2_pixel1.red;
    bool bit1 = img1_pixel1.green != img2_pixel1.green;
    bool bit2 = img1_pixel1.blue != img2_pixel1.blue;
    bool bit3 = img1_pixel1.alpha != img2_pixel1.alpha;
    bool bit4 = img1_pixel2.red != img2_pixel2.red;
    bool bit5 = img1_pixel2.green != img2_pixel2.green;
    bool bit6 = img1_pixel2.blue != img2_pixel2.blue;
    bool bit7 = img1_pixel2.alpha != img2_pixel2.alpha;

    byte b = ((1 << 7) * bit0) | ((1 << 6) * bit1) | ((1 << 5) * bit2) | ((1 << 4) * bit3) |
             ((1 << 3) * bit4) | ((1 << 2) * bit5) | ((1 << 1) * bit6) | ((1 << 0) * bit7);

    return b;
}

// Read the length of the data using the images.
uint32_t read_length(const image& img1, const image& img2) {
    uint32_t length;
    for (size_t i = 0; i < 8; i += 2) {
        rgba img1_pixel1 = img1.pixels[i];
        rgba img1_pixel2 = img1.pixels[i + 1];
        rgba img2_pixel1 = img2.pixels[i];
        rgba img2_pixel2 = img2.pixels[i + 1];

        byte b = get_byte(img1_pixel1, img1_pixel2, img2_pixel1, img2_pixel2);
        // because of little endian
        ((byte*)&length)[3 - i / 2] = b;
    }

    return length;
}

void save_decrypted_data(std::string file, std::vector<byte>& bytes) {
    std::ofstream output{file, std::ios::binary};

    if (!output.is_open()) {
        error_file_can_not_be_read(file);
    }

    output.write((char*)bytes.data(), bytes.size());
}

/*
    Do the decryption pipeline.

    Load key png.
    Load the encrypted png.
    Decrypt the data using the two pngs.
    Save the decypted data into the output file.
*/
void decrypt(std::string key_file, std::string input_file, std::string output_file) {
    auto key = load_png(key_file);
    auto encrypted_png = load_png(input_file);

    auto lenght = read_length(key, encrypted_png);

    std::vector<byte> output;
    output.reserve(lenght);

    for (size_t i = 8; i < 8 + lenght * 2; i += 2) {
        rgba img1_pixel1 = key.pixels[i];
        rgba img1_pixel2 = key.pixels[i + 1];
        rgba img2_pixel1 = encrypted_png.pixels[i];
        rgba img2_pixel2 = encrypted_png.pixels[i + 1];

        byte b = get_byte(img1_pixel1, img1_pixel2, img2_pixel1, img2_pixel2);
        output.push_back(b);
    }
    save_decrypted_data(output_file, output);
}
}  // namespace Private

// Entry point of the encrypting/decrypting
void solve(cli_info info) {
    switch (info.mode) {
        case ENCRYPT:
            Private::encrypt(info.key_file, info.input_file, info.output_file);
            break;

        case DECRYPT:
            Private::decrypt(info.key_file, info.input_file, info.output_file);
            break;
    }
}