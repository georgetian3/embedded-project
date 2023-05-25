#include "audioplayer_gui.h"
#include "audioplayer.h"

int main(int argc, char** argv) {
    return audioplayer_gui(argc, argv);
    AudioPlayer* ap = ap_init();
    int ret = ap_open(ap, argv[1]);
    ap_print_header(ap);
    if (ret) {
        printf("Error: %s\n", ap_errors[ret]);
        return 1;
    }
    //ap_set_speed(ap, 1.5);
    ap_set_timestamp(ap, 5);
    ap_set_volume(ap, 50);
    ap_play(ap);
    char c;
    scanf("%c", &c);
    ap_pause(ap);
    scanf("%c", &c);
}