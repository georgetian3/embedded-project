#include "audioplayer.h"
#define MIXER_NAME "Master"
int ap_init(AudioPlayer* ap) {
    ap->playing = false;
    ap->fp = 0;
    ap->volume = 100;
}

bool ap_is_open(AudioPlayer* ap) {
    return !!ap->fp;
}

bool ap_is_playing(AudioPlayer* ap) {
    return ap->playing;
}



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
        "    format_id    %.4s\n"
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
    ap_init(ap);
    return 0;
}


int ap_play_(AudioPlayer* ap) {

    //if (ap->speed < 0)

    if (ap->timestamp < 0) {
        ap->timestamp = 0;
    }

    int error;

    snd_pcm_t* pcm;


    if (error = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)) {
        printf("error: snd_pcm_open - %s\n", snd_strerror(error));
    }

    if (error = snd_pcm_set_params(
            pcm, SND_PCM_FORMAT_S16_LE,
            SND_PCM_ACCESS_RW_INTERLEAVED,
            ap->header.channels,
            ap->header.sample_rate,
            1, 500000
        )) {

        printf("set error: %s\n", snd_strerror(error));
        return 1;
    }

    size_t chunk_size = 1024 * 1024;
    int factor = 1;
    if (ap->header.bit_depth == 16) {
        factor *= 2;
    }

    factor *= ap->header.channels;

    do {
        for (int i = 0; i < !ap->pause && ap->header.data_size / factor; i += chunk_size) {
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
    } while (!ap->repeat);

    if (error = snd_pcm_close(pcm)) {
        printf("error: snd_pcm_close - %s\n", snd_strerror(error));
        return 1;
    }
    ap->pause = false;
    ap->playing = false;
    return 0;
}

int ap_play(AudioPlayer* ap, double timestamp, double speed, bool blocking) {
    if (ap->playing) {
        return 1;
    }
    ap->playing = true;
    return blocking ? ap_play_(ap) : pthread_create(&ap->thread, NULL, (void*)ap_play_, ap);
}

void ap_pause(AudioPlayer* ap) {
    if (!ap->playing) {
        return 1;
    }
    //pthread_cancel(ap->thread);
    ap->pause = true;
    //ap->playing = false;
}


int ap_set_volume(AudioPlayer* ap, int volume) {
    volume = volume < 0 ? 0 : (volume > 100 ? 100 : volume);

    ap->volume = volume;

    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t* elem ;
    long minv, maxv;
    
    // 初始化snd_mixer_selem_id_t结构体
    snd_mixer_selem_id_alloca(&sid);

    // 打开混音器设备
    if (snd_mixer_open(&handle, 0) < 0) {
        return -1;
    }
    if (snd_mixer_attach(handle, "default") < 0) { 
        snd_mixer_close(handle);
        return -2;
    }
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0) {
        snd_mixer_close(handle);
        return -3;
    }
    if (snd_mixer_load(handle) < 0) {
        snd_mixer_close(handle);
        return -4;
    }

    // 设置混音器的简单元素索引和名称
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, MIXER_NAME);

    // 通过名称找到混音器元素
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        return -5;
    }

    // 获取音量范围
    snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);

    // 根据指定的音量百分比设置音量
    if(volume < 0) volume = 0;
    if(volume > 100) volume = 100;
    snd_mixer_selem_set_playback_volume_all(elem, volume * maxv / 100);

    // 更新AudioPlayer的音量值
    ap->volume = volume;

    // 关闭混音器设备
    snd_mixer_close(handle);
    
    return 0;
}