#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

//#define part2
//#define part3

#ifdef part2
    #include <alsa/asoundlib.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "waveheader.h"

typedef struct {
    char* filename;
    FILE* fp;
    uint8_t* data;
    WaveHeader header;
} AudioPlayer;

// Reads a file, if it is a well-formed wave file parse its header
// Returns: `true` if the file is a well-formed wave file,
//          `false otherwise
// If `false` was returned, all other functions have undefined behavior
int ap_open(AudioPlayer* ap, char* filename);

// Returns pointer to malloc'ed string containing the wave's header information
// User must free pointer
char* ap_get_header_string(AudioPlayer* ap);

// Print header to stdout
void ap_print_header(AudioPlayer* ap);

// Save header to file specified in `filename`
// If `filename` is NULL, then save in same path as wave file
// but with the added extention ".txt"
void ap_save_header(AudioPlayer* ap, char* filename);

// Release AudioPlayer struct init'ed by `ap_open`
bool ap_close(AudioPlayer* ap);


#ifdef part2

int ap_play(AudioPlayer* ap, double timestamp, double speed, bool blocking);
void ap_pause();

#endif

#endif