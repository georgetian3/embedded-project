#include "audioplayer.h"

int ap_open(AudioPlayer* ap, char* filename) {
    ap->filename = strdup(filename);
    ap->fp = fopen(filename, "rb");
    if (!ap->fp) {
        return AP_ERROR_CANNOT_OPEN_FILE;
    }

    size_t bytes_read = fread((void*)&ap->header, 1, sizeof(WaveHeader), ap->fp);
    if (bytes_read != sizeof(WaveHeader)) {
        // Not enough header
        return AP_ERROR_INVALID_WAVE;
    }

    if (!(ap->data = malloc(ap->header.data_size))) {
        return AP_ERROR_MALLOC;
    }

    bytes_read = fread((void*)ap->data, 1, ap->header.data_size, ap->fp);
    if (bytes_read != ap->header.data_size) {
        // Not enough data
        return AP_ERROR_INVALID_WAVE;
    }

    if (ap->header.riff_id   != WAVEHEADER_RIFF_ID     ||
        ap->header.format    != WAVEHEADER_RIFF_FORMAT ||
        ap->header.format_id != WAVEHEADER_FORMAT_ID   ||
        ap->header.data_id   != WAVEHEADER_DATA_ID) {
        // IDs/formats do not match
        return AP_ERROR_INVALID_WAVE;
    }

    return 0;
}

int ap_get_header_string(AudioPlayer* ap, char* str) {
    WaveHeader h = ap->header;
    int chars_written = snprintf(
        str, AP_HEADER_STRING_LEN,
        "RIFF chunk\n"
        "    riff_id      %.4s\n"
        "    chunk_size   %d\n"
        "    format       %.4s\n"
        "Format chunk\n"
        "    format_tag    %.4s\n"
        "    format_size  %d\n"
        "    audio_format %d\n"
        "    channels     %d\n"
        "    sample_rate  %d\n"
        "    byte_rate    %d\n"
        "    block_align  %d\n"
        "    bit_depth    %d\n"
        "Data chunk\n"
        "    data_id      %.4s\n"
        "    data_size    %d",
        (char*)&h.riff_id, h.chunk_size, (char*)&h.format,
        (char*)&h.format_id, h.format_size, h.audio_format,
        h.channels, h.sample_rate, h.byte_rate, h.block_align,
        h.bit_depth, (char*)&h.data_id, h.data_size
    );
    if (chars_written < 200) {
        return AP_ERROR_STRING;
    }
    return 0;
}

int ap_print_header(AudioPlayer* ap) {
    char header_string[AP_HEADER_STRING_LEN];
    int err = ap_get_header_string(ap, header_string);
    if (err) {
        return err;
    }
    printf("%s\n", header_string);
    return 0;
}

int ap_save_header(AudioPlayer* ap, char* filename) {
    if (filename == NULL) {
        size_t wave_filename_length = strlen(ap->filename);
        filename = malloc(wave_filename_length + 5);
        strcpy(filename, ap->filename);
        strcpy((char*)(filename + wave_filename_length), ".txt\0");
    }
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        return AP_ERROR_CANNOT_OPEN_FILE;
    }
    printf("Saved header to %s\n", filename);
    char header_string[AP_HEADER_STRING_LEN];
    int err = ap_get_header_string(ap, header_string);
    if (err) {
        return err;
    }
    fwrite(header_string, 1, strlen(header_string), fp);
    return 0;
}

int ap_close(AudioPlayer* ap) {
    if (ap->filename) {
        free(ap->filename);
        ap->filename = NULL;
    }
    if (ap->data) {
        free(ap->data);
        ap->data = NULL;
    }
    if (ap->fp) {
        fclose(ap->fp);
    }
    return 0;
}

#ifdef part2

int ap_play(AudioPlayer* ap, double timestamp, double speed, bool blocking) {
    if (!blocking) {
        ap_play(ap, timestamp, speed, true);
    }
    snd_pcm_t* pcm;

    int error;

    if (error = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)) {
        printf("error: snd_pcm_open - %s\n", snd_strerror(error));
    }

    snd_pcm_hw_params_t *hw_params;
    // snd_pcm_hw_params_alloca(&hw_params);
    // if (error = snd_pcm_hw_params_any(pcm, hw_params)) {
    //     printf("error: snd_pcm_hw_params_any - %s\n", snd_strerror(error));
    //     return 1;
    // }
    if (error = snd_pcm_set_params(
            pcm, SND_PCM_FORMAT_S16_LE,
            SND_PCM_ACCESS_RW_INTERLEAVED,
            ap->header.format_chunk.NumChannels,
            ap->header.format_chunk.sample_rate, 1, 500000
        )) {

        printf("set error: %s\n", snd_strerror(error));
        return 1;
    }
    // if (error = ) {
    //     printf("alloc error %d\n", error);
    // }
    
    // if (error = snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) {
    //     printf("error: snd_pcm_hw_params_set_access - %s\n", snd_strerror(error));
    //     return 1;
    // }
    // if (error = snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE)) {
    //     printf("error: snd_pcm_hw_params_set_format - %s\n", snd_strerror(error));
    //     return 1;
    // }
    // if (error = snd_pcm_hw_params_set_channels(pcm, hw_params, ap->header.format_chunk.NumChannels)) {
    //     printf("error: snd_pcm_hw_params_set_channels - %s\n", snd_strerror(error));
    //     return 1;
    // }
    // if (error = snd_pcm_hw_params_set_rate(pcm, hw_params, ap->header.format_chunk.sample_rate, 0)) {
    //     printf("error: snd_pcm_hw_params_set_rate - %s\n", snd_strerror(error));
    //     return 1;
    // }
    // // if (error = snd_pcm_hw_params_set_periods(pcm, hw_params, 2, 0)) {
    // //     printf("error: snd_pcm_hw_params_set_periods - %s\n", snd_strerror(error));
    // // }
    // // if (error = snd_pcm_hw_params_set_period_time(pcm, hw_params, header.format_chunk.size / 20, 0)) {
    // //     printf("error: snd_pcm_hw_params_set_period_time - %s\n", snd_strerror(error));
    // // }
    // if (error = snd_pcm_hw_params(pcm, hw_params)) {
    //     printf("error: snd_pcm_hw_params - %s\n", snd_strerror(error));
    //     return 1;
    // }
    // if (error = snd_pcm_prepare(pcm)) {
    //     printf("error: snd_pcm_prepare - %s\n", snd_strerror(error));
    //     return 1;
    // }
    size_t chunk_size = 1024 * 16;
    int factor = 1;
    if (ap->header.format_chunk.bits_per_sample == 16) {
        factor *= 2;
    }
    factor *= ap->header.format_chunk.NumChannels;
    for (int i = 0; i < ap->header.data_chunk.size / factor; i += chunk_size) {
        if ((error = snd_pcm_writei(pcm, (ap->data) + i * factor, chunk_size)) < 0) {
            printf("error: snd_pcm_writei - %s\n", snd_strerror(error));
            return 1;
        } else {
            printf("Frames written: %d\n", error);
        }
    }
    // if (error = snd_pcm_drain(pcm)) {
    //     printf("error: snd_pcm_drain - %s\n", snd_strerror(error));
    // }
    if (error = snd_pcm_close(pcm)) {
        printf("error: snd_pcm_close - %s\n", snd_strerror(error));
        return 1;
    }
    return 0;
}

void ap_pause() {

}

#endif