#ifndef WAVEHEADER_H
#define WAVEHEADER_H

#define WAVEHEADER_RIFF_ID     0x46464952 // "RIFF"
#define WAVEHEADER_RIFF_FORMAT 0x45564157 // "WAVE"
#define WAVEHEADER_FORMAT_ID   0x20746D66 // "fmt "
#define WAVEHEADER_DATA_ID     0x61746164 // "data"

#include <stdint.h>

typedef struct {
    // RIFF chunk
    uint32_t riff_id;       // "RIFF"
    uint32_t chunk_size;    // file size - 8
    uint32_t format;        // "WAVE"
    // Format chunk
    uint32_t format_id;     // "fmt "
    uint32_t format_size;   // PCM = 16
    uint16_t audio_format;  // PCM = 1
    uint16_t channels;      // e.g. 2
    uint32_t sample_rate;   // e.g. 48000Hz
    uint32_t byte_rate;     // sample_rate * bit_depth * channels / 8
    uint16_t block_align;   // bytes per sample = channels * bit_depth / 8
    uint16_t bit_depth;     // bits per sample, e.g. 16
    // Data chunk
    uint32_t data_id;       // "data"
    uint32_t data_size;     // data size = samples * channels * bit_depth / 8
} __attribute__((packed)) WaveHeader;

#endif