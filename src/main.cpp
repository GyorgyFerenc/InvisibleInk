#include "cli.hpp"
#include "ui.hpp"
#include "logic.hpp"

int main(int argc, char const* argv[]) {
    auto info = parse(argc, argv);
    if (info.help) {
        print_help();
    }
    solve(info);
    return 0;
}
