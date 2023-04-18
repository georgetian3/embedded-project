#include "audioplayer.h"

int main(int argc, char** argv) {


    if (argc != 2) {
        printf("Usage: ./audioplayer-xc [filename]\n");
        return 1;
    }

    struct AudioPlayer ap;

    if (open(&ap, argv[1])) {
        return 1;
    }

    print_header(&ap);
    save_header(&ap, NULL);

}