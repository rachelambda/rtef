#include "global.h"
#include "file.h"

int main(int argc, char** argv) {
    if (argc < 2)
        die("not enough args");

    read_files(&argv[1], argc - 1);

    msg("read files");

    check_collissions();
}
