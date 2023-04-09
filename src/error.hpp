#pragma once

#include <string>
struct png_load_error {
    std::string file;
    std::string error;
};
struct png_save_error {
    std::string file;
    std::string error;
};

struct space_error {
    uint32_t data_length;
    uint32_t free_space;
};

struct file_error {
    std::string file;
};