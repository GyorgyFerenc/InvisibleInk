#pragma once
#include <iostream>

namespace Private {
const std::string RESET = "\e[0m";
const std::string RED = "\e[1;31m";

void print_error(std::string text) {
    std::cout << RED << "[ERROR] " << RESET << text << std::endl;
}
}  // namespace Private

enum error_code : int {
    NO_ARGUMENT = 1,
    PNG_LOAD,
    PNG_SAVE,
    NOT_ENOUGH_SPACE,
};

// Shows the error message and exists the program
void error_no_argument() {
    Private::print_error("No arguemnt provided run with -h for help");
    exit(error_code::NO_ARGUMENT);
}

// Shows error message and exists the program
void error_loading_png(std::string file, std::string png_error) {
    Private::print_error("error at loading png: " + file);
    Private::print_error(png_error);
    exit(error_code::PNG_LOAD);
}
// Shows error message and exists the program
void error_saving_png(std::string file, std::string png_error) {
    Private::print_error("error at saving png: " + file);
    Private::print_error(png_error);
    exit(error_code::PNG_SAVE);
}

void error_not_enough_space(uint data_length, uint free_space) {
    Private::print_error("Not enough space");
    Private::print_error("Space needed: " + data_length);
    Private::print_error("Space availble: " + free_space);
    exit(error_code::NOT_ENOUGH_SPACE);
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
    exit(0);
}
