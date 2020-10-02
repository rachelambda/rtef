#include <stdio.h>
#include <elf.h>

#include "global.h"
#include "file.h"

elf_file* infiles;
size_t infilecnt;

void verify_file(const char* path) {
    msg("validating file '%s'", path);

    FILE* fp = fopen(path, "rb");

    if (!fp)
        die("Unable to read file '%s'", path);

    Elf64_Ehdr ehdr;

    if (fread(&ehdr, 1, sizeof ehdr, fp) != sizeof ehdr)
        msg("Unable to read header from file '%s'", path);

    if (ehdr.e_ident[EI_MAG0] != 0x7f || ehdr.e_ident[EI_MAG1] != 'E' ||
            ehdr.e_ident[EI_MAG2] != 'L' || ehdr.e_ident[EI_MAG3] != 'F')
        die("Not an ELF file '%s'", path);

    if (ehdr.e_ident[EI_CLASS] != ELFCLASS64)
        die("Not a 64 bit file '%s'", path);

    if (ehdr.e_ident[EI_DATA] == ELFDATANONE)
        die("Unknown data format in file '%s'", path);

    if (ehdr.e_ident[EI_VERSION] == EV_NONE)
        die("Invalid ELF version in file '%s'", path);

    if (ehdr.e_type != ET_REL)
        die("Not a relocatable file '%s'", path);

    if (ehdr.e_machine != EM_X86_64)
        die("Not a x86_64 file '%s'", path);

    if (ehdr.e_version == EV_NONE)
        die("Invalid ELF version in file '%s'", path);
}

void read_files(const char* paths[], int pathc) {
    infiles = malloc(pathc * (sizeof(const char*)));
    infilecnt = pathc;

    if (!infiles)
        die("malloc failed");

    for (int i = 0; i < pathc; i++) {
        infiles[i].path = paths[i];
        /* TODO: read file data here */
    }
}
