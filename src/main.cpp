#include "io.hpp"
#include "logic.hpp"

void encrypt_branch(cli_info& info);
void decrypt_branch(cli_info& info);

int main(int argc, char const* argv[]) {
    auto info = parse(argc, argv);

    switch (info.task) {
        case HELP:
            print_help();
            break;
        case ENCRYPT:
            encrypt_branch(info);
            break;
        case DECRYPT:
            decrypt_branch(info);
            break;
    }

    return exit_code::SUCCES;
}
/*
    Do the encryption pipeline.

    Load key png.
    Load data to be encrypted.
    Encrypt the data
    Save image.
*/
void encrypt_branch(cli_info& info) {
    try {
        auto key = load_png(info.key_file);

        auto data =
            read_data_to_be_encrypted(info.input_file);

        encrypt(key, data);
        save_png(key, info.output_file);

    } catch (space_error e) {
        error_not_enough_space(e.data_length, e.free_space);
    } catch (file_error e) {
        error_file_can_not_be_opened(e.file);
    } catch (png_save_error e) {
        error_saving_png(e.file, e.error);
    } catch (png_load_error e) {
        error_loading_png(e.file, e.error);
    }
}

/*
    Do the decryption pipeline.

    Load key png.
    Load the encrypted png.
    Decrypt the data using the two pngs.
    Save the decypted data into the output file.
*/
void decrypt_branch(cli_info& info) {
    try {
        auto key = load_png(info.key_file);
        auto encrypted_png = load_png(info.input_file);

        auto output = decrypt(key, encrypted_png);

        save_decrypted_data(info.output_file, output);
    } catch (space_error e) {
        error_not_enough_space(e.data_length, e.free_space);
    } catch (file_error e) {
        error_file_can_not_be_opened(e.file);
    } catch (png_save_error e) {
        error_saving_png(e.file, e.error);
    } catch (png_load_error e) {
        error_loading_png(e.file, e.error);
    }
}
