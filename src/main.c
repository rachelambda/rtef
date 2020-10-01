#include "global.h"
#include "verify.h"

int main(int argc, char** argv) {
    if (argc < 2)
        die("not enough args");

    for (int i = 1; i < argc; i++)
        verify_file(argv[i]);
}
