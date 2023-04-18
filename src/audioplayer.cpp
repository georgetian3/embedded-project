#include "audioplayer.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


std::string AudioPlayer::int_to_string(const uint32_t i) const {
    std::string str(4, 0);
    auto ptr = (uint32_t*)str.data();
    *ptr = i;
    return str;
}

void AudioPlayer::delete_data() {
    if (data != nullptr) {
        delete[] data;
        data = nullptr;
    }
}


AudioPlayer::~AudioPlayer() {
    delete_data();
}

bool AudioPlayer::read(const std::filesystem::path& filename) {
    delete_data();
    header = {};
    audio_path = filename;
    std::ifstream wav_file(audio_path);
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
        header.riff_chunk.ID     == RIFF_CHUNK_ID   &&
        header.riff_chunk.Format == RIFF_WAVE_FORMAT &&
        header.format_chunk.ID   == FORMAT_CHUNK_ID &&
        header.data_chunk.ID     == DATA_CHUNK_ID;
}

WaveHeader AudioPlayer::get_header() {
    return header;
}
void AudioPlayer::get_data(uint8_t* buf) {
    memcpy(buf, data, header.data_chunk.Size);
}
std::string AudioPlayer::header_string() const {
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
void AudioPlayer::print_header() const {
    std::cout << header_string();
}
void AudioPlayer::save_header(std::filesystem::path header_path) const {
    if (header_path.empty()) {
        header_path = audio_path;
        header_path.replace_extension("txt");
    }
    std::ofstream fout(header_path);
    fout << header_string();
}