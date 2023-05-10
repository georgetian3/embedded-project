#include "audioplayer.h"

#include <string.h>

const char help[] =
"Audio Player\n"
"o: open file\n"
"i: print file and playback info\n"
"p: toggle play/pause\n"
"r: toggle repeat\n"
"h: print this help\n"
"v: set volume\n"
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
    int ret;

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
            if (ret = ap_open(&ap, "example.wav")) {
                printf("Error: %s\n", ap_errors[ret]);
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
        } else if (streq(cmd, "v")) {
            printf("Volume: ");
            gl(cmd, MAX_CMD_LEN);
            int volume;
            if ((ret = sscanf(cmd, "%d", &volume)) == EOF) {
                printf("Invalid volume\n");
            } else if (ret = ap_set_volume(&ap, volume)) {
                if (ret < 0) {
                    printf("Set volume error: %s\n", snd_strerror(ret));
                } else {
                    printf("???\n");
                }
            } else {
                printf("Volume set to: %d\n", ap.volume);
            }
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
        } else {
            printf("Invalid command\n");
        }

    }

    ap_close(&ap);

}