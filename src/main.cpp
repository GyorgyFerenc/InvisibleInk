#include "cli.hpp"
#include "log.hpp"
#include "logic.hpp"

int main(int argc, char const* argv[]) {
    auto info = parse(argc, argv);
    if (info.help) {
        print_help();
    }
    solve(info);

    return exit_code::SUCCES;
}
