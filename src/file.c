#include <stdio.h>
#include <elf.h>

#include "global.h"
#include "file.h"

elf_file* infiles;
size_t infilecnt;

unsigned int has_entry = 0;

void read_files(char** paths, int pathc) {
    infiles = malloc(pathc * (sizeof(const char*)));
    infilecnt = pathc;

    if (!infiles)
        die("malloc failed");

    for (int i = 0; i < pathc; i++) {
        infiles[i].path = paths[i];

        FILE* fp = fopen(infiles[i].path, "rb");

        if (!fp)
            die("Unable to read file '%s'", infiles[i].path);

        msg("Reading file '%s'", infiles[i].path);

        if (fread(&infiles[i].ehdr, 1, sizeof(Elf64_Ehdr), fp)
                != sizeof(Elf64_Ehdr))
            msg("Unable to read header from file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_ident[EI_MAG0] != 0x7f || infiles[i].ehdr.e_ident[EI_MAG1] != 'E' ||
                infiles[i].ehdr.e_ident[EI_MAG2] != 'L' || infiles[i].ehdr.e_ident[EI_MAG3] != 'F')
            die("Not an ELF file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_ident[EI_CLASS] != ELFCLASS64)
            die("Not a 64 bit file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_ident[EI_DATA] == ELFDATANONE)
            die("Unknown data format in file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_ident[EI_VERSION] == EV_NONE)
            die("Invalid ELF version in file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_type != ET_REL)
            die("Not a relocatable file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_machine != EM_X86_64)
            die("Not a x86_64 file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_version == EV_NONE)
            die("Invalid ELF version in file '%s'", infiles[i].path);

        if (infiles[i].ehdr.e_entry) {
            if (has_entry) {
                die("Multiple entry points");
            } else {
                has_entry = 1;
            }
        }

        if (infiles[i].ehdr.e_shnum >= SHN_LORESERVE)
            /* TODO handle first entry of sh for shnum here */

        if (!infiles[i].ehdr.e_shnum)
            die("No section header table in file '%s'", infiles[i].path);

        size_t sh_bytes = sizeof(Elf64_Shdr) * infiles[i].ehdr.e_shnum;

        infiles[i].shdrs = malloc(sh_bytes);

        if (!infiles[i].shdrs)
            die("Unable to allocate memory");

        fseek(fp, infiles[i].ehdr.e_shoff, SEEK_SET);

        if (fread(infiles[i].shdrs, 1, sh_bytes, fp) != sh_bytes)
            msg("Unable to read section headers from file '%s'", infiles[i].path);

        for (int n = 0; n < infiles[i].ehdr.e_shnum; n++)
            msg("%d/%d: %d", n, infiles[i].ehdr.e_shnum, infiles[i].shdrs[n].sh_type);
    }
}
