#pragma once

#include <cstdint>
#include <fstream>
#include <iterator>

#include "error.hpp"
#include "image.hpp"
// Reads it as byte array
std::vector<byte> read_data_to_be_encrypted(
    std::string file) {
    std::ifstream input(file, std::ios::binary);

    if (!input.is_open()) {
        throw file_error{file};
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
    result.insert(result.begin(),
                  std::istream_iterator<byte>(input),
                  std::istream_iterator<byte>());

    return result;
}

// gets the the bit from byte b in the position pos
// bits are numbered from most significant to least
bool get_bit(const byte& b, uint pos) {
    uint mask = 1 << (7 - pos);
    return (b & mask) != 0;
}

void encrypt_bit(byte& channel, bool bit) {
    // potential optimization:
    // x + (x == 255)*(-2) + (x != 255)
    if (channel < 255)
        channel += bit;
    else
        channel -= bit;
}

// encrypts 1 byte of data into two pixels
void encrypt_into_pixels(rgba& pixel1,
                         rgba& pixel2,
                         byte  data) {
    encrypt_bit(pixel1.red, get_bit(data, 0));
    encrypt_bit(pixel1.green, get_bit(data, 1));
    encrypt_bit(pixel1.blue, get_bit(data, 2));
    encrypt_bit(pixel1.alpha, get_bit(data, 3));

    encrypt_bit(pixel2.red, get_bit(data, 4));
    encrypt_bit(pixel2.green, get_bit(data, 5));
    encrypt_bit(pixel2.blue, get_bit(data, 6));
    encrypt_bit(pixel2.alpha, get_bit(data, 7));
}

// encrypts the length into the first 4 byte, which is the
// first 8 pixel
void encrypt_length_of_data(image& img, uint32_t lenght) {
    byte first_byte =
        ((byte*)&lenght)[0];  // Getting the first byte of
                              // length
    byte second_byte =
        ((byte*)&lenght)[1];  // Getting the second byte of
                              // length
    byte third_byte =
        ((byte*)&lenght)[2];  // Getting the third byte of
                              // length
    byte fourth_byte =
        ((byte*)&lenght)[3];  // Getting the fourth byte of
                              // length

    // we need to reverse it because of little endian
    std::vector<byte> data{
        {fourth_byte, third_byte, second_byte, first_byte}};
    uint i = 0;

    for (auto&& one_byte : data) {
        auto& pixel1 = img.pixels[i];
        auto& pixel2 = img.pixels[i + 1];
        i += 2;

        encrypt_into_pixels(pixel1, pixel2, one_byte);
    }
}

/*
    Encrypts data into the image

    Check for enough space in the photo.
    Encrypt the data into the image.
*/
void encrypt(image& key, std::vector<byte> data) {
    uint32_t length_of_data = data.size();

    uint free_space = key.pixels.size() / 2;
    // Every 2 pixel can hold 1 byte of information
    // First 4 byte is lenght of data
    bool enough_space = length_of_data + 4 <= free_space;
    if (!enough_space) {
        throw space_error{length_of_data, free_space};
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
}

// Extractes the byte from the pixels.
byte get_byte(rgba& img1_pixel1,
              rgba& img1_pixel2,
              rgba& img2_pixel1,
              rgba& img2_pixel2) {
    bool bit0 = img1_pixel1.red != img2_pixel1.red;
    bool bit1 = img1_pixel1.green != img2_pixel1.green;
    bool bit2 = img1_pixel1.blue != img2_pixel1.blue;
    bool bit3 = img1_pixel1.alpha != img2_pixel1.alpha;
    bool bit4 = img1_pixel2.red != img2_pixel2.red;
    bool bit5 = img1_pixel2.green != img2_pixel2.green;
    bool bit6 = img1_pixel2.blue != img2_pixel2.blue;
    bool bit7 = img1_pixel2.alpha != img2_pixel2.alpha;

    byte b = ((1 << 7) * bit0) | ((1 << 6) * bit1) |
             ((1 << 5) * bit2) | ((1 << 4) * bit3) |
             ((1 << 3) * bit4) | ((1 << 2) * bit5) |
             ((1 << 1) * bit6) | ((1 << 0) * bit7);

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

        byte b = get_byte(img1_pixel1,
                          img1_pixel2,
                          img2_pixel1,
                          img2_pixel2);
        // because of little endian
        ((byte*)&length)[3 - i / 2] = b;
    }

    return length;
}

void save_decrypted_data(std::string        file,
                         std::vector<byte>& bytes) {
    std::ofstream output{file, std::ios::binary};

    if (!output.is_open()) {
        throw file_error{file};
    }

    output.write((char*)bytes.data(), bytes.size());
}

/*
    Decrypt the data using the two pngs.
*/
std::vector<byte> decrypt(image& key,
                          image& encrypted_png) {
    auto lenght = read_length(key, encrypted_png);

    std::vector<byte> output;
    output.reserve(lenght);

    for (size_t i = 8; i < 8 + lenght * 2; i += 2) {
        rgba img1_pixel1 = key.pixels[i];
        rgba img1_pixel2 = key.pixels[i + 1];
        rgba img2_pixel1 = encrypted_png.pixels[i];
        rgba img2_pixel2 = encrypted_png.pixels[i + 1];

        byte b = get_byte(img1_pixel1,
                          img1_pixel2,
                          img2_pixel1,
                          img2_pixel2);
        output.push_back(b);
    }

    return output;
}

// Entry point of the encrypting/decrypting
// void solve(cli_info info) {
//     switch (info.mode) {
//         case ENCRYPT:
//             Private::encrypt(info.key_file,
//                              info.input_file,
//                              info.output_file);
//             break;

//         case DECRYPT:
//             Private::decrypt(info.key_file,
//                              info.input_file,
//                              info.output_file);
//             break;
//     }
// }