#include "audioplayer.h"
#include <dirent.h>

#define min(x, y) (x < y) ? x : y

#define error_ret(expr) if (ret = expr) {return ret;}

struct AudioPlayer {
    char* dir;
    char** files;
    int file_count;
    char* filename;
    FILE* fp;
    uint8_t* data;
    WaveHeader header;
    double timestamp;
    double speed;
    int volume;
    pthread_t thread;
    bool repeat;
    bool is_playing;
    bool pause;
};


static int clamp(int val, int low, int high) {
    return val < low ? low : val > high ? high : val;
}


AudioPlayer* ap_init() {
    AudioPlayer* ap = malloc(sizeof(AudioPlayer));
    ap->dir = NULL;
    ap-> files = NULL;
    ap->file_count = 0;
    ap->filename = NULL;
    ap->fp = 0;
    ap->data = NULL;
    ap->timestamp = 0;
    ap->speed = 1;
    ap->volume = 100;
    ap->thread = 0;
    ap->repeat = false;
    ap->is_playing = false;
    ap->pause = false;
    return ap;
}

int ap_destroy(AudioPlayer* ap) {
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
    free(ap);
    return 0;
}


int ap_scan_dir(AudioPlayer* ap, const char* path) {
    if (ap->files != NULL) {
        for (int i = 0; i < ap->file_count; i++) {
            if (ap->files[i] != NULL) {
                free(ap->files[i]);
            }
        }
        free(ap->files);
    }
    ap->file_count = 0;
    int file_count = 0;
    struct dirent* entry;
    DIR* dir = opendir(path);
    if (dir == NULL) {
        g_print("dir == null\n");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }
    ap->files = malloc(sizeof(char*) * file_count);
    dir = opendir(path);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) {
            continue;
        }
        char cmd[1024];
        sprintf(cmd, "ffprobe %s 2>&1 | grep \"Stream .*: Audio: \" | wc -l", entry->d_name);
        FILE* res = popen(cmd, "r");
        int lines;
        fscanf(res, "%d", &lines);
        if (lines > 0) {
            ap->files[ap->file_count++] = strdup(entry->d_name);
        }
    }
}

int ap_audio_file_count(AudioPlayer* ap) {
    return ap->file_count;
}

const char** ap_get_audio_files(AudioPlayer* ap) {
    return ap->files;
}

bool ap_is_open(AudioPlayer* ap) {
    return !!ap->fp;
}

bool ap_is_playing(AudioPlayer* ap) {
    return ap->is_playing;
}

int ap_open(AudioPlayer* ap, char* filename) {
    // TODO: memory leak here
    //ap_close(ap);

    char cmd[1024];
    sprintf(cmd, "ffmpeg -y -i \"%s\" -f s16le \"%s.audioplayer.converted.wav\" > /dev/null 2>&1", filename, filename);

    FILE* ffmpeg = popen(cmd, "r");
    pclose(ffmpeg);

    char converted_filename[1024];
    sprintf(converted_filename, "%s.audioplayer.converted.wav", filename);

    g_print("Converted filename: %s\n", converted_filename);

    ap->filename = strdup(filename);
    ap->fp = fopen(converted_filename, "rb");
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


int ap_play_(AudioPlayer* ap) {

    if (ap->timestamp < 0) {
        ap->timestamp = 0;
    }

    int ret;

    snd_pcm_t* pcm;

    error_ret(snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));

    error_ret(
        snd_pcm_set_params(
            pcm, SND_PCM_FORMAT_S16_LE,
            SND_PCM_ACCESS_RW_INTERLEAVED,
            ap->header.channels,
            ap->header.sample_rate,
            1, 100000
        )
    )

    size_t chunk_size = ap->header.byte_rate / 10;


    do {
        for (int i = 0; !ap->pause && i < ap->header.data_size; i += chunk_size * ap->header.channels * 2) {

            if ((ret = snd_pcm_writei(
                    pcm, (ap->data) + i,
                    min(ap->header.data_size - i, chunk_size)
                )) < 0) {

                return ret;
            } else {
                //printf("Frames written: %d\n", ret);
            }
        }
        // if (error = snd_pcm_drain(pcm)) {
        //     printf("error: snd_pcm_drain - %s\n", snd_strerror(error));
        // }
    } while (!ap->pause && ap->repeat);

    error_ret(snd_pcm_close(pcm))
    ap->pause = false;
    ap->is_playing = false;

    return 0;
}

int ap_play_pause(AudioPlayer* ap) {
    if (!ap_is_open(ap)) {
        return AP_ERROR_FILE_NOT_OPEN;
    }
    if (ap->is_playing) {
        ap->pause = true;
        return 0;
    } else {
        ap->is_playing = true;
        g_print("creating thread and playing\n");
        return pthread_create(&ap->thread, NULL, (void*)ap_play_, ap);
    }
}


double ap_duration(AudioPlayer* ap) {
    return 69;
}

double ap_get_speed(AudioPlayer* ap) {
    return ap->speed;
}

int ap_set_speed(AudioPlayer* ap, double speed) {


}

double ap_get_timestamp(AudioPlayer* ap) {
    return ap->timestamp;
}

int ap_set_timestamp(AudioPlayer* ap, double timestamp) {


}

int ap_get_volume(AudioPlayer* ap) {
    return ap->volume;
}

int ap_set_volume(AudioPlayer* ap, int volume) {
    volume = clamp(volume, 0, 100);
    ap->volume = volume;

    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;

    int ret;
    
    snd_mixer_selem_id_alloca(&sid);

    error_ret(snd_mixer_open(&handle, 0))
    error_ret(snd_mixer_attach(handle, "default"))
    error_ret(snd_mixer_selem_register(handle, NULL, NULL))
    error_ret(snd_mixer_load(handle))

    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        return 1;
    }

    long minv, maxv;
    snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);

    error_ret(snd_mixer_selem_set_playback_volume_all(elem, minv + volume * maxv / 10000))

    snd_mixer_close(handle);
    
    return 0;
}
