#pragma once
#include <string>

#include "ui.hpp"

enum encryption_mode {
    ENCRYPT,
    DECRYPT,
};

// Holds information about the given command line parameters
struct cli_info {
    bool            help;
    encryption_mode mode = ENCRYPT;
    std::string     input_file;
    std::string     output_file;
    std::string     key_file;
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
            info.help = true;
        } else if (arg == "-e") {
            info.mode = ENCRYPT;
        } else if (arg == "-d") {
            info.mode = DECRYPT;
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
