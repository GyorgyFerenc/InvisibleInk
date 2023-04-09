#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "image.hpp"

namespace Private {
const std::string RESET = "\e[0m";
const std::string RED = "\e[1;31m";

// Print the sylized error msg
void print_error(std::string text) {
    std::cout << RED << "[ERROR] " << RESET << text
              << std::endl;
}
}  // namespace Private

enum exit_code : int {
    SUCCES = 0,

    // Error codes
    NO_ARGUMENT = 1,
    PNG_LOAD,
    PNG_SAVE,
    NOT_ENOUGH_SPACE,
    FILE_CAN_NOT_BE_OPENED,
};

// Shows the error message and exists the program
void error_no_argument() {
    Private::print_error(
        "No arguemnt provided run with -h for help");
    exit(exit_code::NO_ARGUMENT);
}

// Shows error message and exists the program
void error_loading_png(std::string file,
                       std::string png_error) {
    Private::print_error("error at loading png: " + file);
    Private::print_error(png_error);
    exit(exit_code::PNG_LOAD);
}
// Shows error message and exists the program
void error_saving_png(std::string file,
                      std::string png_error) {
    Private::print_error("error at saving png: " + file);
    Private::print_error(png_error);
    exit(exit_code::PNG_SAVE);
}
// Shows error message and exists the program
void error_not_enough_space(uint32_t data_length,
                            uint32_t free_space) {
    Private::print_error("Not enough space");
    Private::print_error("Space needed: " + data_length);
    Private::print_error("Space availble: " + free_space);
    exit(exit_code::NOT_ENOUGH_SPACE);
}
// Shows error message and exists the program
void error_file_can_not_be_opened(std::string file) {
    Private::print_error(file + " can not be opened");
    exit(exit_code::FILE_CAN_NOT_BE_OPENED);
}
// Prints the help text and exists the program
void print_help() {
    std::string help = R"(    invisible_ink flags*

    description:
        Encrypts data into a png file.

    flags:
        -h -> Prints this text.

        -e -> Sets the mode to encryption. The default mode is encryption.

        -d -> Sets the mode to decryptdion.

        -i file -> In encryption mode specifies the file where the data is read from
            for encryption.
                    In decryption mode specifies the png file which holds the encrypted
            data.

        -o file -> In encryption mode specifies the new png file that will hold the 
            encrypted data.
                    In decryption mode specifies the file where encrypted data will be
            written.

        -k file -> Specifies the key to be used for encryption/decryption.
    )";
    std::cout << help << std::endl;
    exit(exit_code::SUCCES);
}

enum task_type {
    HELP,  // Print the help
    ENCRYPT,
    DECRYPT,
};

// Holds information about the given command line parameters
struct cli_info {
    task_type   task = ENCRYPT;
    std::string input_file;
    std::string output_file;
    std::string key_file;
};

// Parses the given arguments
cli_info parse(int argc, char const* argv[]) {
    if (argc <= 1) {
        error_no_argument();
    }

    cli_info info{};

    for (int i = 1; i < argc; i++) {
        std::string arg{argv[i]};

        if (arg == "-h") {
            info.task = HELP;
        } else if (arg == "-e") {
            info.task = ENCRYPT;
        } else if (arg == "-d") {
            info.task = DECRYPT;
        } else if (arg == "-i") {
            i++;
            info.input_file = std::string{argv[i]};
        } else if (arg == "-o") {
            i++;
            info.output_file = std::string{argv[i]};
        } else if (arg == "-k") {
            i++;
            info.key_file = std::string{argv[i]};
        }
    }

    return info;
}

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

void save_decrypted_data(std::string        file,
                         std::vector<byte>& bytes) {
    std::ofstream output{file, std::ios::binary};

    if (!output.is_open()) {
        throw file_error{file};
    }

    output.write((char*)bytes.data(), bytes.size());
}
