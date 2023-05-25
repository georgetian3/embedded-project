#include "audioplayer_gui.h"
#include "audioplayer.h"

int main(int argc, char** argv) {
    //return audioplayer_gui(argc, argv);
    g_print("here\n");

    AudioPlayer* ap = ap_init();
    g_print("here0.5\n");

    int ret = ap_open(ap, argv[1]);
    g_print("here1\n");

    ap_print_header(ap);
    if (ret) {
        printf("Error: %s\n", ap_errors[ret]);
        return 1;
    }
    ap_set_speed(ap, 2);
    ap_set_volume(ap, 20);
    ap_play(ap);
    char c;
    scanf("%c", &c);
}