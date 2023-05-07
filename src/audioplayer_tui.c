#include "audioplayer.h"

#include <string.h>

const char help[] =
"Audio Player\n"
"o: open file\n"
"i: print file and playback info\n"
"p: toggle play/pause\n"
"r: toggle repeat\n"
"h: print this help\n"
"+: volume + 10\n"
"-: volume - 10\n"
"q: quit\n";

const char yes[] = "yes";
const char no[] = "no";


#define MAX_CMD_LEN 1024

void gl(char* buffer, int count) {
    fgets(buffer, MAX_CMD_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

#define streq(x, y) strcmp(x, y) == 0

void ap_tui() {

    AudioPlayer ap;
    ap_init(&ap);
    int error;

    char cmd[MAX_CMD_LEN];
    printf(help);

    while (1) {
        printf("> ");
        gl(cmd, MAX_CMD_LEN);
        if (streq(cmd, "h")) {
            printf(help);
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
            gl(cmd, MAX_CMD_LEN);
            if (error = ap_open(&ap, "example.wav")) {
                printf("Error: %s\n", ap_errors[error]);
            } else {
                printf("Opened\n");
            }
        } else if (streq(cmd, "q")) {
            break;
        } else if (streq(cmd, "r")) {
            ap.repeat = !ap.repeat;
            printf(ap.repeat ? "Repeating\n" : "No repeating\n");
        } else if (!ap_is_open(&ap)) {
            printf("No file opened\n");
        } else if (streq(cmd, "+")) {
            ap_set_volume(&ap, ap.volume + 10);
            printf("Volume: %d\n", ap.volume);
        } else if (streq(cmd, "-")) {
            ap_set_volume(&ap, ap.volume - 10);
            printf("Volume: %d\n", ap.volume);
        } else if (streq(cmd, "i")) {
            ap_print_header(&ap);
            printf("\n");
            printf("Playing   %s\n", ap_is_playing(&ap) ? yes : no);
            printf("Timestamp %.2fs\n", ap.timestamp);
            printf("Speed     %.1fx\n", ap.speed);
            printf("Repeating %s\n", ap.repeat ? yes : no);
        } else if (streq(cmd, "p")) {
            if (ap_is_playing(&ap)) {
                printf("Pausing\n");
                ap_pause(&ap);
            } else {
                printf("Playing\n");
                ap_play(&ap, ap.timestamp, ap.speed, false);
            }
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
        } else if (streq(cmd, "o")) {
            printf("Filename: ");
        } else {
            printf("Invalid command\n");
        }

    }

}