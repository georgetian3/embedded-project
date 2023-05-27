#include "audioplayer.h"
#include "stretch.h"
#include <math.h>
#include <dirent.h>
#include <gtk/gtk.h>

#define min(x, y) (x < y) ? x : y

#define error_ret(expr) if (ret = expr) {return ret;}
#define MAX_CMD_LEN 1024
#define AP_FRAMES_PER_CHUNK 256

#define AP_CONVERTED_FILE ".audioplayer.converted.wav"

struct AudioPlayer {
    char** audio_filenames;
    int audio_file_count;
    char* open_filename;
    FILE* fp;
    uint8_t* data;
    WaveHeader header;
    double duration;
    double timestamp;
    int at_byte;
    bool at_byte_changed;
    pthread_mutex_t byte_lock;
    double speed;
    int volume;
    pthread_t thread;
    bool repeat;
    bool is_playing;
    bool pause;
    bool is_open;
    int16_t* speed_buffers[3];
};

#define clamp(val, low, high) val < low ? low : val > high ? high : val

AudioPlayer* ap_init() {
    AudioPlayer* ap = malloc(sizeof(AudioPlayer));
    ap->audio_filenames = NULL;
    ap->audio_file_count = 0;
    ap->open_filename = NULL;
    ap->fp = 0;
    ap->data = NULL;
    ap->timestamp = 0;
    ap->at_byte = 0;
    ap->at_byte_changed = false;
    ap->speed = 1;
    ap->volume = 100;
    ap->thread = 0;
    ap->repeat = false;
    ap->is_playing = false;
    ap->pause = false;

    for (int i = 0; i < 3; i++) {
        ap->speed_buffers[i] = NULL;
    }

    if (pthread_mutex_init(&ap->byte_lock, NULL)) {
        printf("\n mutex init has failed\n");
        exit(1);
    }

    return ap;
}

int ap_close(AudioPlayer* ap) {
    ap->is_open = false;
    ap->at_byte = 0;
    ap_pause(ap);
    if (ap->thread) {
        pthread_join(ap->thread, NULL);
        ap->thread = 0;
    }
    if (ap->open_filename) {
        free(ap->open_filename);
        ap->open_filename = NULL;
    }
    remove(AP_CONVERTED_FILE);
    if (ap->data) {
        free(ap->data);
        ap->data = NULL;
    }
    for (int i = 0; i < 3; i++) {
        if (ap->speed_buffers[i]) {
            free(ap->speed_buffers[i]);
            ap->speed_buffers[i] = NULL;
        }
    }
    if (ap->fp) {
        fclose(ap->fp);
        ap->fp = 0;
    }
    return 0;
}

int to_even(int n) {
    return n + (n % 2);
}


int ap_destroy(AudioPlayer* ap) {
    ap_close(ap);
    if (ap->audio_filenames) {
        for (int i = 0; i < ap->audio_file_count; i++) {
            free(ap->audio_filenames[i]);
        }
        free(ap->audio_filenames);
    }
    free(ap);
    return 0;
}

bool ap_file_contains_audio(const char* filename) {
    char cmd[MAX_CMD_LEN];
    sprintf(cmd, "ffprobe %s 2>&1 | grep \"Stream .*: Audio: \" | wc -l", filename);
    FILE* res = popen(cmd, "r");
    int lines;
    fscanf(res, "%d", &lines);
    pclose(res);
    return lines > 0;
}

int ap_scan_dir(AudioPlayer* ap, const char* path) {
    ap->audio_file_count = 0;
    DIR* dir = opendir(path);
    if (dir == NULL) {
        return 1;
    }
    ap->audio_filenames = malloc(sizeof(char*) * MAX_CMD_LEN);
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && ap->audio_file_count < MAX_CMD_LEN) {
        if (entry->d_type == DT_REG && entry->d_name[0] != '.' && ap_file_contains_audio(entry->d_name)) {
            ap->audio_filenames[ap->audio_file_count++] = strdup(entry->d_name);
        }
    }
    return 0;
}

int ap_audio_file_count(AudioPlayer* ap) {
    return ap->audio_file_count;
}

const char** ap_get_audio_filenames(AudioPlayer* ap) {
    return (const char**)ap->audio_filenames;
}

bool ap_get_repeat(AudioPlayer* ap) {
    return ap->repeat;
}
int ap_set_repeat(AudioPlayer* ap, bool repeat) {
    ap->repeat = repeat;
}

bool ap_is_open(AudioPlayer* ap) {
    return ap->is_open;
}

bool ap_is_playing(AudioPlayer* ap) {
    return ap->is_playing;
}

int ap_open(AudioPlayer* ap, const char* filename) {

    // don't open if already open
    if (ap->open_filename && strcmp(filename, ap->open_filename) == 0) {
        return 0;
    }

    ap_close(ap);

    ap->open_filename = strdup(filename);

    char cmd[MAX_CMD_LEN];
    sprintf(cmd, "ffmpeg -y -v quiet -i \"%s\" -fflags +bitexact %s", filename, AP_CONVERTED_FILE);
    system(cmd);

    FILE* fp = fopen(AP_CONVERTED_FILE, "rb");
    if (!fp) {
        return AP_ERROR_CANNOT_OPEN_FILE;
    }

    size_t bytes_read = fread((void*)&ap->header, 1, sizeof(WaveHeader), fp);
    if (bytes_read != sizeof(WaveHeader)) {
        // Not enough header
        return AP_ERROR_INVALID_WAVE;
    }



    ap_print_header(ap);

    if (!(ap->data = malloc(ap->header.data_size))) {
        return AP_ERROR_MALLOC;
    }


    bytes_read = fread((void*)ap->data, 1, ap->header.data_size, fp);
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


    ap->duration = 1.0 * ap->header.data_size / ap->header.byte_rate;

    int upper_frequency = 200, lower_frequency = 100;
    int min_period = ap->header.sample_rate / upper_frequency;
    int max_period = ap->header.sample_rate / lower_frequency;

    int total_samples = ap->header.data_size / (ap->header.channels * ap->header.bit_depth / 8);
    StretchHandle stretcher = stretch_init(min_period, max_period, ap->header.channels, STRETCH_FAST_FLAG);
    int max_expected_samples = stretch_output_capacity(stretcher, total_samples, 2);

    double speeds[] = {0.5, 1.5, 2.0};

    for (int i = 0; i < 3; i++) {
        ap->speed_buffers[i] = malloc(max_expected_samples * ap->header.channels * ap->header.bit_depth / 8);
        stretch_samples(
            stretcher,
            (const int16_t*)ap->data,
            total_samples,
            ap->speed_buffers[i], 1 / speeds[i]
        );
    }

    stretch_deinit(stretcher);
    ap->is_open = true;
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
        size_t wave_filename_length = strlen(ap->open_filename);
        filename = malloc(wave_filename_length + 5);
        strcpy(filename, ap->open_filename);
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

    ap->is_playing = true;

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
            0, 100000
        )
    )


    int frame_size = ap->header.channels * (ap->header.bit_depth / 8);
    int chunk_size = frame_size * AP_FRAMES_PER_CHUNK;

    do {
        while (ap->at_byte < ap->header.data_size && !ap->pause) {

            uint8_t* data;
            double speed = ap->speed;
            if (speed == 0.5) { 
                data = (uint8_t*)ap->speed_buffers[0];
            } else if (speed == 1.5) { 
                data = (uint8_t*)ap->speed_buffers[1];
            } else if (speed == 2.0) { 
                data = (uint8_t*)ap->speed_buffers[2];
            } else {
                data = ap->data;
            }

            //ret = snd_pcm_writei(pcm, data + ap->at_byte, AP_FRAMES_PER_CHUNK);
            ret = snd_pcm_writei(pcm, data + to_even(ap->at_byte / speed), AP_FRAMES_PER_CHUNK / speed);

            if (ret < 0) {
                printf("play_ ret: %s\n", snd_strerror(ret));
                return ret;
            } else {
                //printf("Frames written: %d\n", ret);
            }

            pthread_mutex_lock(&ap->byte_lock);
            if (!ap->at_byte_changed) {
                ap->at_byte += chunk_size;
            } else {
                ap->at_byte_changed = false;
            }
            pthread_mutex_unlock(&ap->byte_lock);
        }
        if (ret = snd_pcm_drain(pcm)) {
            printf("error: snd_pcm_drain - %s\n", snd_strerror(ret));
        }
    } while (!ap->pause && ap->repeat);

    error_ret(snd_pcm_close(pcm));

    ap->is_playing = false;

    return 0;
}

int ap_play(AudioPlayer* ap) {
    if (!ap_is_open(ap)) {
        return AP_ERROR_FILE_NOT_OPEN;
    }
    if (ap->is_playing) {
        return 0;
    }

    pthread_mutex_lock(&ap->byte_lock);
    if (ap->at_byte >= ap->header.data_size) {
        ap->at_byte = 0;
    }
    pthread_mutex_unlock(&ap->byte_lock);

    int ret = pthread_create(&ap->thread, NULL, (void*)ap_play_, ap);
    while (!ap->is_playing);
    return ret;
}

int ap_pause(AudioPlayer* ap) {
    if (!ap_is_open(ap)) {
        return AP_ERROR_FILE_NOT_OPEN;
    }
    ap->pause = true;
    if (ap->thread) {
        pthread_join(ap->thread, NULL);
    }
    ap->pause = false;
    ap->thread = 0;
}



double ap_duration(AudioPlayer* ap) {
    if (!ap_is_open(ap)) {
        return -1;
    }
    return ap->duration;
}
double ap_get_speed(AudioPlayer* ap) {
    return ap->speed;
}
int ap_set_speed(AudioPlayer* ap, double speed) {
    ap->speed = speed;
}
double ap_get_timestamp(AudioPlayer* ap) {
    if (!ap_is_open(ap)) {
        return -1;
    }
    return 1.0 * ap->at_byte / ap->header.byte_rate;
    //return ap->timestamp;
}
int ap_set_timestamp(AudioPlayer* ap, double timestamp) {
    if (!ap_is_open(ap)) {
        return -1;
    }
    timestamp = clamp(timestamp, 0, ap->duration);
    pthread_mutex_lock(&ap->byte_lock);
    ap->at_byte = to_even(timestamp * ap->header.byte_rate);
    ap->at_byte_changed = true;
    pthread_mutex_unlock(&ap->byte_lock);
    return 0;
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
