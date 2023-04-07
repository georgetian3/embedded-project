#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define RIFF_CHUNK_ID   0x52494646
#define FORMAT_CHUNK_ID 0x57415645
#define DATA_CHUNK_ID   0x64617461

struct RIFFChunk {
    uint32_t ID;
    uint32_t Size;
    uint32_t Format;
} __attribute__((packed));

struct FormatChunk {
    uint32_t ID;
    uint32_t Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
} __attribute__((packed));

struct DataChunk {
    uint32_t ID;
    uint32_t Size;
} __attribute__((packed));

struct WaveHeader {
    RIFFChunk riff_chunk;
    FormatChunk format_chunk;
    DataChunk data_chunk;
} __attribute__((packed));


class WaveReader {

private:

    WaveHeader header = {};
    uint8_t* data = nullptr;
    std::filesystem::path wave_path;

    std::string int_to_string(uint32_t i) {
        std::string str(4, 0);
        auto ptr = (uint32_t*)str.data();
        *ptr = i;
        return str;
    }

    void delete_data() {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }

public:

    WaveReader() {

    }

    // Reads a file, if it is a well-formed wave file parse its header
    // Returns: `true` if the file is a well-formed wave file,
    //          `false otherwise
    // If `false` was returned, all other functions have undefined behavior
    bool read(const std::filesystem::path& filename) {
        delete_data();
        header = {};
        wave_path = filename;
        std::ifstream wav_file(wave_path);
        wav_file.read((char*)&header, sizeof(WaveHeader));

        if (wav_file.eof()) {
            return false;
        }

        data = new uint8_t[header.data_chunk.Size];
        wav_file.read((char*)data, header.data_chunk.Size);

        if (wav_file.eof()) {
            return false;
        }

        return
            header.riff_chunk.ID   == RIFF_CHUNK_ID   &&
            header.format_chunk.ID == FORMAT_CHUNK_ID &&
            header.data_chunk.ID   == DATA_CHUNK_ID;
    }

    // Returns the wave file header
    WaveHeader get_header() {
        return header;
    }
    // Copies the audio data into `buf`
    // User must ensure `buf` is sufficiently long
    void get_data(uint8_t* buf) {
        memcpy(buf, data, header.data_chunk.Size);
    }
    // Returns: formatted wave file header 
    std::string header_string() {
        std::ostringstream str;
        str <<
            "RIFFChunk" <<
                "\n    ID            " << int_to_string(header.riff_chunk.ID) <<
                "\n    Size          " << header.riff_chunk.Size <<
                "\n    Format        " << int_to_string(header.riff_chunk.Format) << 
            "\nFormatChunk" <<
                "\n    ID            " << int_to_string(header.format_chunk.ID) <<
                "\n    Size          " << header.format_chunk.Size <<
                "\n    AudioFormat   " << header.format_chunk.AudioFormat <<
                "\n    NumChannels   " << header.format_chunk.NumChannels <<
                "\n    SampleRate    " << header.format_chunk.SampleRate <<
                "\n    ByteRate      " << header.format_chunk.ByteRate <<
                "\n    BlockAlign    " << header.format_chunk.BlockAlign <<
                "\n    BitsPerSample " << header.format_chunk.BitsPerSample <<
            "\nDataChunk" <<
                "\n    ID            " << int_to_string(header.data_chunk.ID) <<
                "\n    Size          " << header.data_chunk.Size << '\n';
        return str.str();
    }
    // Print the formatted wave file header
    void print_header() {
        std::cout << header_string();
    }
    // Save the formatted wave file header in file `header_path`
    // If no path is provided, it is saved at the same path as the
    // input wave file, but replacing the suffix with "txt"
    // E.g.: /foo/bar/audio.wav -> /foo/bar/audio.txt
    void save_header(std::filesystem::path header_path = std::filesystem::path()) {
        if (header_path.empty()) {
            header_path = wave_path.replace_extension("txt");
        }
        std::ofstream fout(header_path);
        fout << header_string();
    }

    ~WaveReader() {
        delete_data();
    }


};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./music [filename]\n";
        return 0;
    }
    WaveReader reader;
    reader.read(argv[1]);
    reader.read(argv[1]);

    reader.print_header();
    reader.save_header();
}