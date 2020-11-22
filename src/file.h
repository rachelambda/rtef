#ifndef __VERIFY_H
#define __VERIFY_H

#include <stdio.h>
#include <elf.h>

struct sec_def;
struct elf_file;

typedef struct elf_file {
    /* file pointer to object file */
    FILE* fp;
    /* file header */
    Elf64_Ehdr ehdr;
    /* section headers */
    Elf64_Shdr* shdrs;
    /* section definittions for shdrs, and indecies */
    size_t* sec_indecies;
    struct sec_def** sec_defs;
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
    /* relocation addend entries correspondin sections */
    char** relsecs;
    /* relocation entry count */
    size_t relcnt;
    /* relocation addend entries */
    Elf64_Rela* relas;
    /* relocation addend entries correspondin sections */
    char** relasecs;
    /* relocation addend entry count */
    size_t relacnt;
    /* contents of section string table section */
    char* secstr;
    /* contents of symbol string table section */
    char* symstr;
} elf_file;

/* struct used for collecting symbol definitions for merging */
typedef struct {
    char* name;
    size_t defs;
    Elf64_Sym** syms;
    size_t symcnt;
} sym_def;

/* struct used for collecting section headers for merging */
typedef struct sec_def {
    char* name;

    struct {
        Elf64_Shdr* sec;
        size_t offset;

        /* THIS CURRENTLY ASSUMES ALL RELAS ARE PART OF A SINGLE .TEXT */
        struct elf_file* file;
    }* secs;
    size_t seccnt;

    size_t offset;
    uint64_t align;

    Elf64_Addr addr;

    size_t memory_size;
    uint8_t* memory;

    /* permission flags for section, used for creating program header */
    uint32_t p_flags;
    uint32_t sh_type;
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
