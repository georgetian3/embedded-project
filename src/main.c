#include "audioplayer.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: ./audioplayer <filename>\n");
        return 1;
    }

    AudioPlayer ap;
    int error;
    
    if (error = ap_open(&ap, argv[1])) {
        printf("%s\n", ap_errors[error]);
        return 1;
    }

    ap_print_header(&ap);
    ap_save_header(&ap, NULL);


#ifdef part2
    ap_play(&ap, 0, 1, true);
#endif

    ap_close(&ap);

}