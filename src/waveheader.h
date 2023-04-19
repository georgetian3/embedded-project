#ifndef WAVEHEADER_H
#define WAVEHEADER_H

#define WAVEHEADER_RIFF_CHUNK_ID    0x46464952 // "RIFF"
#define WAVEHEADER_RIFF_WAVE_FORMAT 0x45564157 // "WAVE"
#define WAVEHEADER_FORMAT_CHUNK_ID  0x20746D66 // "fmt "
#define WAVEHEADER_DATA_CHUNK_ID    0x61746164 // "data"

#include <stdint.h>

typedef struct {
    uint32_t ID;                            // "RIFF"
    uint32_t Size;                          // file size - 8
    uint32_t Format;                        // "WAVE"
} __attribute__((packed)) RIFFChunk;

typedef struct {
    uint32_t ID;                            // "fmt "
    uint32_t Size;                          //
    uint16_t AudioFormat;                   //
    uint16_t NumChannels;                   // e.g. 2
    uint32_t SampleRate;                    // e.g. 48000Hz
    uint32_t ByteRate;                      //
    uint16_t BlockAlign;                    //
    uint16_t BitsPerSample;                 // e.g. 16
} __attribute__((packed)) FormatChunk;

typedef struct {
    uint32_t ID;                            // "data"
    uint32_t Size;                          // data size
} __attribute__((packed)) DataChunk;

typedef struct {
    RIFFChunk riff_chunk;
    FormatChunk format_chunk;
    DataChunk data_chunk;
} __attribute__((packed)) WaveHeader;

#endif