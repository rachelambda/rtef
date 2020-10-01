#include <stdio.h>
#include <elf.h>

#include "global.h"
#include "verify.h"

void verify_file(const char* path) {
    FILE* fp = fopen(path, "rb");

    if (!fp)
        die("Unable to read file '%s'", path);

    unsigned char e_ident[EI_NIDENT];

    if (fread(e_ident, EI_NIDENT, 1, fp) != EI_NIDENT)
        die("Unable to read header from file '%s'", path);

    if (e_ident[EI_MAG0] != 0x7f || e_ident[EI_MAG1] != 'E' ||
            e_ident[EI_MAG2] != 'L' || e_ident[EI_MAG3] != 'F')
        die("Not an ELF file '%s'", path);

    if (e_ident[EI_CLASS] != ELFCLASS64)
        die("Not a 64 bit file '%s'", path);

    if (e_ident[EI_DATA] == ELFDATANONE)
        die("Unknown data format in file '%s'", path);

    if (e_ident[EI_VERSION] == EV_NONE)
        die("Invalid ELF version in file '%s'", path);

    /* TODO check for it being a relocatabale and stuff */
}
