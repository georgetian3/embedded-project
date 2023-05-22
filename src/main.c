#include "audioplayer_gui.h"
#include "audioplayer.h"

int main(int argc, char** argv) {
    //return audioplayer_gui(argc, argv);
    AudioPlayer* ap = ap_init();
    int ret;
    if (ret = ap_open(ap, "example.wav")) {
        printf("Error: %s\n", ap_errors[ret]);
        ap_print_header(ap);
        return 1;
    }
    ap_play_pause(ap);
    char buf[1];
    scanf("%s", buf);
}