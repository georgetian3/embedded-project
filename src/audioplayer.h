#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "waveheader.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>

#define AP_HEADER_STRING_LEN        1024
#define AP_ERROR_MALLOC             1
#define AP_ERROR_CANNOT_OPEN_FILE   2
#define AP_ERROR_INVALID_WAVE       3
#define AP_ERROR_STRING             4
#define AP_ERROR_FILE_NOT_OPEN      5

static char ap_errors[][128] = {
    "OK",
    "malloc error",
    "Cannot open file",
    "Invalid wave",
    "String error",
    "File not open",
};




struct AudioPlayer;
typedef struct AudioPlayer AudioPlayer;

// Initialize values in `AudioPlayer` struct
AudioPlayer* ap_init();
// Release AudioPlayer struct init'ed by `ap_open`
int ap_destroy(AudioPlayer* ap);

int ap_scan_dir(AudioPlayer* ap, const char* dir);

int ap_audio_file_count(AudioPlayer* ap);
const char** ap_get_audio_filenames(AudioPlayer* ap);


// Reads a file, if it is a well-formed wave file
// parse its header and save in `ap`
int ap_open(AudioPlayer* ap, const char* filename);
int ap_close(AudioPlayer* ap);
bool ap_is_open(AudioPlayer* ap);

bool ap_file_contains_audio(const char* filename);

// Save formatted header in `str`
// `str` must be at least `AP_HEADER_STRING_LEN` long
int ap_get_header_string(AudioPlayer* ap, char* str);

// Print header to stdout
int ap_print_header(AudioPlayer* ap);

// Save header to file specified in `filename`
// If `filename` is NULL, then save in same path as wave file
// but with the added extention ".txt"
int ap_save_header(AudioPlayer* ap, char* filename);

bool ap_get_repeat(AudioPlayer* ap);
int ap_set_repeat(AudioPlayer* ap, bool repeat);


int ap_play_pause(AudioPlayer* ap);
bool ap_is_playing(AudioPlayer* ap);

double ap_get_speed(AudioPlayer* ap);
int ap_set_speed(AudioPlayer* ap, double speed);


double ap_duration(AudioPlayer* ap);

double ap_get_timestamp(AudioPlayer* ap);
// timestamp will be clamped into [0, duration]
int ap_set_timestamp(AudioPlayer* ap, double timestamp);

// volume will be clamped into [0, 100]
int ap_get_volume(AudioPlayer* ap);
int ap_set_volume(AudioPlayer* ap, int volume);




#endif