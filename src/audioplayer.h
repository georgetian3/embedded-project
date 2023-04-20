#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

//#define part2
//#define part3

#include "waveheader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef part2
    #include <alsa/asoundlib.h>
#endif


#define AP_HEADER_STRING_LEN        1024
#define AP_ERROR_MALLOC             1
#define AP_ERROR_CANNOT_OPEN_FILE   2
#define AP_ERROR_INVALID_WAVE       3
#define AP_ERROR_STRING             4

static char ap_errors[][128] = {
    "OK",
    "malloc error",
    "Cannot open file",
    "Invalid wave",
    "String error",
};

typedef struct {
    char* filename;
    FILE* fp;
    uint8_t* data;
    WaveHeader header;
} AudioPlayer;

// Reads a file, if it is a well-formed wave file
// parse its header and save in `ap`
int ap_open(AudioPlayer* ap, char* filename);

// Save formatted header in `str`
// `str` must be at least `AP_HEADER_STRING_LEN` long
int ap_get_header_string(AudioPlayer* ap, char* str);

// Print header to stdout
int ap_print_header(AudioPlayer* ap);

// Save header to file specified in `filename`
// If `filename` is NULL, then save in same path as wave file
// but with the added extention ".txt"
int ap_save_header(AudioPlayer* ap, char* filename);

// Release AudioPlayer struct init'ed by `ap_open`
int ap_close(AudioPlayer* ap);


#ifdef part2
int ap_play(AudioPlayer* ap, double timestamp, double speed, bool blocking);
void ap_pause();
#endif

#endif