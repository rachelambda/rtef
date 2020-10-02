#ifndef __VERIFY_H
#define __VERIFY_H

#include <stdio.h>
#include <elf.h>

typedef struct {
    char* path;
    Elf64_Ehdr edhr;
    Elf64_Shdr* shdrs;
    size_t shdrcnt;
    Elf64_Sym* syms;
    size_t symcnt;
    Elf64_Rel* rels;
    size_t relcnt;
    Elf64_Rela* relas;
    size_t relacnt;
    unsigned char** sections;
    size_t* section_lens;
    size_t sectioncnt;
} elf_file;

/*
  Checks that the file at the given path
  is an x86_64 ELF relocatable file.
 */
void verify_file(const char* path);

/*
  Reads pathc amount of files in array paths.
 */
void read_files(const char* paths[], int pathc);

#endif
