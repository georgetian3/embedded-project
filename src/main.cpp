#include "argparse.hpp"
#include "audioplayer.hpp"

#include <iostream>

int main(int argc, char** argv) {

    argparse::ArgumentParser program("music");
    program.add_argument("filename")
        .help("Filename of the wave file");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto filename = program.get("filename");


    AudioPlayer player;
    if (!player.read(filename)) {
        std::cout << "\"" << filename << "\" is not a valid wave file\n";
        return 1;
    }


    int dl = 32;
    std::cout << std::string(dl, '=') << '\n' << filename
        << '\n' << std::string(dl, '-') << '\n';
    player.print_header();
    player.save_header();
    std::cout << std::string(dl, '=') << '\n';

#ifdef part2
    player.play();
#endif

}