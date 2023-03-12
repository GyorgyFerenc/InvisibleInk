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
    auto key = load_png(info.key_file);
    auto data = read_data_to_be_encrypted(info.input_file);

    encrypt(key, data);
    save_png(key, info.output_file);
}

/*
    Do the decryption pipeline.

    Load key png.
    Load the encrypted png.
    Decrypt the data using the two pngs.
    Save the decypted data into the output file.
*/
void decrypt_branch(cli_info& info) {
    auto key = load_png(info.key_file);
    auto encrypted_png = load_png(info.input_file);

    auto output = decrypt(key, encrypted_png);

    save_decrypted_data(info.output_file, output);
}
