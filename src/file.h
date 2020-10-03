#ifndef __VERIFY_H
#define __VERIFY_H

#include <stdio.h>
#include <elf.h>

typedef struct {
    const char* path;
    Elf64_Ehdr ehdr;
    Elf64_Shdr* shdrs;
    size_t shdrcnt;
    Elf64_Sym* syms;
    size_t symcnt;
    Elf64_Rel* rels;
    size_t relcnt;
    Elf64_Rela* relas;
    size_t relacnt;
} elf_file;

/* get the endian of a file */
#define F_ENDIAN(f) f.ehdr.e_ident[EI_DATA]
/* get the endian of a file pointer */
#define P_ENDIAN(f) f->ehdr.e_ident[EI_DATA]

/* Reads pathc amount of files in array paths */
/* and then collects ELF data from them. */
void read_files(char** paths, int pathc);

#endif
