#define part1
#define part2
//#define part3

#ifdef part2
    #include <alsa/asoundlib.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "waveheader.h"

struct AudioPlayer {
    char* filename;
    struct WaveHeader header;
    uint8_t* data;
    FILE* fp;
};



int open(struct AudioPlayer* ap, char* filename) {
    ap->filename = strdup(filename);
    ap->fp = fopen(filename, "rb");
    if (!ap->fp) {
        printf("File not found\n");
        return 1;
    }

    int err;

    err = fseek(ap->fp, 0, SEEK_END);
    if (err) {
        printf("Cannot seek\n");
        return 2;
    }
    long file_size = ftell(ap->fp);
    if (file_size == -1) {
        printf("ftell fail\n");
        return 3;
    }
    if (file_size < sizeof(struct WaveHeader)) {
        printf("Invalid wave file - header not long enough\n");
        return 4;
    }
    err = fseek(ap->fp, 0, SEEK_SET);
    if (err) {
        printf("Cannot seek\n");
        return 2;
    }

    size_t bytes_read = fread((void*)&ap->header, 1, sizeof(struct WaveHeader), ap->fp);
    if (bytes_read != sizeof(struct WaveHeader)) {
        printf("Invalid wave file - cannot read enough header\n");
        return 4;
    }

    if (ap->header.riff_chunk.Size + 8 != file_size) {
        printf("Invalid wave file: file_size + 8 != riff_chunk.Size: %d != %d\n", ap->header.riff_chunk.Size, file_size);
        return 4;
    }

    /* printf("%d %d %d %d\n", sizeof(struct WaveHeader), ap->header.data_chunk.Size, sizeof(struct WaveHeader) + ap->header.data_chunk.Size,  file_size);

    if (sizeof(struct WaveHeader) + ap->header.data_chunk.Size != file_size) {
        printf("Invalid wave file - data not long enough\n");
        return 4;
    } */

    ap->data = malloc(ap->header.data_chunk.Size);
    if (!ap->data) {
        printf("Invalid malloc\n");
        return 5;
    }

    bytes_read = fread((void*)ap->data, 1, ap->header.data_chunk.Size, ap->fp);
    if (bytes_read != ap->header.data_chunk.Size) {
        printf("Invalid wave file - cannot read enough data\n");
        return 4;
    }

    if (ap->header.riff_chunk.ID     != RIFF_CHUNK_ID    ||
        ap->header.riff_chunk.Format != RIFF_WAVE_FORMAT ||
        ap->header.format_chunk.ID   != FORMAT_CHUNK_ID  ||
        ap->header.data_chunk.ID     != DATA_CHUNK_ID) {
        
        printf("Invalid wave file - IDs and formats incorrect\n");
        return 4;
    }

    return 0;
}

char* get_header_string(struct AudioPlayer* ap) {
    struct WaveHeader h = ap->header;
    char* header_string = malloc(1024 * 1024);
    int chars_written = sprintf(
        header_string,
        "RIFFChunk\n"
        "    ID            %.4s\n"
        "    Size          %d\n"
        "    Format        %.4s\n"
        "FormatChunk\n"
        "    ID            %.4s\n"
        "    Size          %d\n"
        "    AudioFormat   %d\n"
        "    NumChannels   %d\n"
        "    SampleRate    %d\n"
        "    ByteRate      %d\n"
        "    BlockAlign    %d\n"
        "    BitsPerSample %d\n"
        "DataChunk\n"
        "    ID            %.4s\n"
        "    Size          %d",
        &h.riff_chunk.ID, h.riff_chunk.Size, &h.riff_chunk.Format,
        &h.format_chunk.ID, h.format_chunk.Size, h.format_chunk.AudioFormat,
        h.format_chunk.NumChannels, h.format_chunk.SampleRate,
        h.format_chunk.ByteRate, h.format_chunk.BlockAlign,
        h.format_chunk.BitsPerSample,
        &h.data_chunk.ID, h.data_chunk.Size
    );
    return header_string;
}

void print_header(struct AudioPlayer* ap) {
    char* header_string = get_header_string(ap);
    printf("%s\n", header_string);
    free(header_string);
}

void save_header(struct AudioPlayer* ap, char* filename) {
    if (filename == NULL) {
        size_t wave_filename_length = strlen(ap->filename);
        filename = malloc(wave_filename_length + 4);
        strcpy(filename, ap->filename);
        strcpy((char*)(filename + wave_filename_length), ".txt");
    }
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("Cannot open %s for writing\n", filename);
        return;
    }
    printf("Saved header to %s\n", filename);
    char* header_string = get_header_string(ap);
    fwrite(header_string, 1, strlen(header_string), fp);
    free(header_string);
}

bool close(struct AudioPlayer* ap) {
    if (ap->filename != NULL) {
        free(ap->filename);
        ap->filename = NULL;
    }
    if (ap->data != NULL) {
        free(ap->data);
        ap->data = NULL;
    }
}



// Reads a file, if it is a well-formed wave file parse its header
// Returns: `true` if the file is a well-formed wave file,
//          `false otherwise
// If `false` was returned, all other functions have undefined behavior





#ifdef part2

bool play(struct AudioPlayer* ap, double timestamp = 0.0, double speed = 1.0, bool blocking = true) {
    if (!blocking) {
        play(timestamp, speed, true);
    }
    snd_pcm_t* pcm;

    int error;
    printf("snd_pcm_open pcm\n");

    if (error = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)) {
        printf("error: snd_pcm_open - %s\n", snd_strerror(error));
    }

    snd_pcm_hw_params_t *hw_params;
    printf("snd_pcm_hw_params_alloca pcm\n");

    snd_pcm_hw_params_alloca(&hw_params);
    if (error = snd_pcm_hw_params_any(pcm, hw_params)) {
        printf("error: snd_pcm_hw_params_any - %s\n", snd_strerror(error));
    }
    if (error = snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) {
        printf("error: snd_pcm_hw_params_set_access - %s\n", snd_strerror(error));
    }
    if (error = snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE)) {
        printf("error: snd_pcm_hw_params_set_format - %s\n", snd_strerror(error));
    }
    if (error = snd_pcm_hw_params_set_channels(pcm, hw_params, header.format_chunk.NumChannels)) {
        printf("error: snd_pcm_hw_params_set_channels - %s\n", snd_strerror(error));
    }
    if (error = snd_pcm_hw_params_set_rate(pcm, hw_params, header.format_chunk.SampleRate, 0)) {
        printf("error: snd_pcm_hw_params_set_rate - %s\n", snd_strerror(error));
    }
    // if (error = snd_pcm_hw_params_set_periods(pcm, hw_params, 2, 0)) {
    //     printf("error: snd_pcm_hw_params_set_periods - %s\n", snd_strerror(error));
    // }
    // if (error = snd_pcm_hw_params_set_period_time(pcm, hw_params, header.format_chunk.Size / 20, 0)) {
    //     printf("error: snd_pcm_hw_params_set_period_time - %s\n", snd_strerror(error));
    // }
    if (error = snd_pcm_hw_params(pcm, hw_params)) {
        printf("error: snd_pcm_hw_params - %s\n", snd_strerror(error));
    }
    // if (error = snd_pcm_prepare(pcm)) {
    //     printf("error: snd_pcm_prepare - %s\n", snd_strerror(error));
    // }
    size_t chunk_size = 1024;
    int factor = 1;
    if (header.format_chunk.BitsPerSample == 16) {
        factor *= 2;
    }
    factor *= header.format_chunk.NumChannels;
    for (int i = 0; i < header.data_chunk.Size / factor; i += chunk_size) {
        if ((error = snd_pcm_writei(pcm, data + i * factor, chunk_size)) < 0) {
            printf("error: snd_pcm_writei - %s\n", snd_strerror(error));
        } else {
            printf("Frames written: " << error << '\n');
        }
    }
    // if (error = snd_pcm_drain(pcm)) {
    //     printf("error: snd_pcm_drain - %s\n", snd_strerror(error));
    // }
    if (error = snd_pcm_close(pcm)) {
        printf("error: snd_pcm_close - %s\n", snd_strerror(error));
    }
    return true;
}

void pause() {

}

#endif