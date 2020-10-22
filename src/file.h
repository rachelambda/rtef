#ifndef __VERIFY_H
#define __VERIFY_H

#include <stdio.h>
#include <elf.h>

typedef struct {
    /* file pointer to object file */
    FILE* fp;
    /* file header */
    Elf64_Ehdr ehdr;
    /* section headers */
    Elf64_Shdr* shdrs;
    /* section string tab section header */
    Elf64_Shdr secstrtab;
    /* Symbol string tab section header */
    Elf64_Shdr symstrtab;
    /* symbols */
    Elf64_Sym* syms;
    /* symbol count */
    size_t symcnt;
    /* relocation entries */
    Elf64_Rel* rels;
    /* relocation entry count */
    size_t relcnt;
    /* relocation addend entries */
    Elf64_Rela* relas;
    /* relocation addend entry count */
    size_t relacnt;
    /* contents of section string table section */
    char* secstr;
    /* contents of symbol string table section */
    char* symstr;
} elf_file;

typedef struct {
    char* name;
    size_t defs;
    Elf64_Sym** syms;
    size_t symcnt;
} sym_def;

typedef struct {
    char* name;
    Elf64_Shdr** secs;
    size_t seccnt;
} sec_def;
/* get the endian of a file */
#define F_ENDIAN(f) f.ehdr.e_ident[EI_DATA]
/* get the endian of a file pointer */
#define P_ENDIAN(f) f->ehdr.e_ident[EI_DATA]
/* find out if a symbol is defined */
#define SYM_IS_DEF(sym) (sym.st_shndx != SHN_UNDEF)

/* Reads pathc amount of files in array paths */
/* and then collects ELF data from them. */
/* resulting data is put in infiles array */
void read_files(char** paths, int pathc);

/* read symbol and relocation entries and then use these */
/* in order to check for collisions in files */
void check_collisions();

/* use collected data to write an executable file */
void create_exec(char* filename);

#endif
