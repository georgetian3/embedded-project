//#define part2
//#define part3

#include "audioplayer.h"

int main(int argc, char** argv) {


    if (argc != 2) {
        printf("Usage: ./audioplayer-[debug|xc] filename\n");
        return 1;
    }

    struct AudioPlayer ap;

    if (ap_open(&ap, argv[1])) {
        return 1;
    }

    ap_print_header(&ap);
    ap_save_header(&ap, NULL);
    ap_close(&ap);

}