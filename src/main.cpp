#include "argparse.hpp"
#include "audioplayer.hpp"

#include <iostream>

int main(int argc, char** argv) {

    std::cout << "main\n";

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

    std::cout << "filename: " << filename << "\n";


    AudioPlayer player;
    if (!player.read(filename)) {
        std::cout << "\"" << filename << "\" is not a valid wave file\n";
        return 1;
    }


    int dl = 32;
    std::cout << std::string(dl, '=') << '\n' << filename
        << '\n' << std::string(dl, '-') << '\n';
    std::cout << "before print\n";
    player.print_header();
    std::cout << "before save\n";
    player.save_header();
    std::cout << std::string(dl, '=') << '\n';

    std::cout << "before play\n";


#ifdef part2
    player.play();
#endif

}