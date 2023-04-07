#include "argparse.hpp"
#include "wavereader.hpp"

#include <iostream>

int main(int argc, char** argv) {

    argparse::ArgumentParser program("music");
    program.add_argument("filename")
        .help("Filename of the wave file");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto filename = program.get("filename");

    int dl = 32;
    
    WaveReader reader;
    if (reader.read(filename)) {
        std::cout << std::string(dl, '=') << '\n' << filename
            << '\n' << std::string(dl, '-') << '\n';
        reader.print_header();
        reader.save_header();
        std::cout << std::string(dl, '=') << '\n';
    } else {
        std::cout << "\"" << filename << "\" is not a valid wave file\n";
    } 

}