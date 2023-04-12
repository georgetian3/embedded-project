#ifndef WAVEHEADER_HPP
#define WAVEHEADER_HPP

#include <cstdint>

#define RIFF_CHUNK_ID   0x46464952 // RIFF
#define FORMAT_CHUNK_ID 0x20746D66 // fmt 
#define DATA_CHUNK_ID   0x61746164 // data

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

#endif