#include <filesystem>
#include <string>


#include "waveheader.hpp"

class WaveReader {

private:

    WaveHeader header = {};
    uint8_t* data = nullptr;
    std::filesystem::path wave_path;
    std::string int_to_string(const uint32_t i) const;
    void delete_data();

public:

    WaveReader();
    ~WaveReader();

    // Reads a file, if it is a well-formed wave file parse its header
    // Returns: `true` if the file is a well-formed wave file,
    //          `false otherwise
    // If `false` was returned, all other functions have undefined behavior
    bool read(const std::filesystem::path& filename);

    WaveHeader get_header();
    // Copies the audio data into `buf`
    // User must ensure `buf` is sufficiently long
    void get_data(uint8_t* buf);

    // Returns: formatted wave file header 
    std::string header_string() const;
    // Print the formatted wave file header
    void print_header() const;

    void save_header(std::filesystem::path header_path = std::filesystem::path()) const;

};

