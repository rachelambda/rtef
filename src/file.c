#include <stdio.h>
#include <string.h>
#include <elf.h>

#include "global.h"
#include "file.h"

elf_file* infiles;
size_t infilecnt;

unsigned int has_entry = 0;

const char* STR_SHT_NULL = "SHT_NULL";
const char* STR_SHT_PROGBITS = "SHT_PROGBITS";
const char* STR_SHT_SYMTAB = "SHT_SYMTAB";
const char* STR_SHT_STRTAB = "SHT_STRTAB";
const char* STR_SHT_RELA = "SHT_RELA";
const char* STR_SHT_HASH = "SHT_HASH";
const char* STR_SHT_DYNAMIC = "SHT_DYNAMIC";
const char* STR_SHT_NOTE = "SHT_NOTE";
const char* STR_SHT_NOBITS = "SHT_NOBITS";
const char* STR_SHT_REL = "SHT_REL";
const char* STR_SHT_SHLIB = "SHT_SHLIB";
const char* STR_SHT_DYNSYM = "SHT_DYNSYM";

const char* shdrtype_str(uint32_t type_num) {
    switch (type_num) {
        case SHT_NULL:
            return STR_SHT_NULL;
        case SHT_PROGBITS:
            return STR_SHT_PROGBITS;
        case SHT_SYMTAB:
            return STR_SHT_SYMTAB;
        case SHT_STRTAB:
            return STR_SHT_STRTAB;
        case SHT_RELA:
            return STR_SHT_RELA;
        case SHT_HASH:
            return STR_SHT_HASH;
        case SHT_DYNAMIC:
            return STR_SHT_DYNAMIC;
        case SHT_NOTE:
            return STR_SHT_NOTE;
        case SHT_NOBITS:
            return STR_SHT_NOBITS;
        case SHT_REL:
            return STR_SHT_REL;
        case SHT_SHLIB:
            return STR_SHT_SHLIB;
        case SHT_DYNSYM:
            return STR_SHT_DYNSYM;
    }
    return 0;
}

void read_files(char** paths, int pathc) {
    xmalloc(infiles, pathc * (sizeof(elf_file)));
    infilecnt = pathc;

    for (int i = 0; i < pathc; i++) {
        infiles[i].fp = fopen(paths[i], "rb");

        if (!infiles[i].fp)
            die("Unable to read file '%s'", paths[i]);

        msg("=== Reading file '%s' ===", paths[i]);

        /* START VERIFY FILE */

        if (fread(&infiles[i].ehdr, 1, sizeof(Elf64_Ehdr), infiles[i].fp)
                != sizeof(Elf64_Ehdr))
            msg("Unable to read header from file '%s'", paths[i]);

        if (infiles[i].ehdr.e_ident[EI_MAG0] != 0x7f || infiles[i].ehdr.e_ident[EI_MAG1] != 'E' ||
                infiles[i].ehdr.e_ident[EI_MAG2] != 'L' || infiles[i].ehdr.e_ident[EI_MAG3] != 'F')
            die("Not an ELF file '%s'", paths[i]);

        if (infiles[i].ehdr.e_ident[EI_CLASS] != ELFCLASS64)
            die("Not a 64 bit file '%s'", paths[i]);

        if (infiles[i].ehdr.e_ident[EI_DATA] == ELFDATANONE)
            die("Unknown data format in file '%s'", paths[i]);

        if (infiles[i].ehdr.e_ident[EI_VERSION] == EV_NONE)
            die("Invalid ELF version in file '%s'", paths[i]);

        if (infiles[i].ehdr.e_type != ET_REL)
            die("Not a relocatable file '%s'", paths[i]);

        if (infiles[i].ehdr.e_machine != EM_X86_64)
            die("Not a x86_64 file '%s'", paths[i]);

        if (infiles[i].ehdr.e_version == EV_NONE)
            die("Invalid ELF version in file '%s'", paths[i]);

        if (infiles[i].ehdr.e_entry) {
            if (has_entry) {
                die("Multiple entry points");
            } else {
                has_entry = 1;
            }
        }

        /* END FILE VALIDATION */

        if (infiles[i].ehdr.e_shnum >= SHN_LORESERVE)
            die("unhandeled code path");
            /* TODO handle first entry of sh for shnum here */

        if (!infiles[i].ehdr.e_shnum)
            die("No section header table in file '%s'", paths[i]);

        size_t sh_bytes = sizeof(Elf64_Shdr) * infiles[i].ehdr.e_shnum;

        xmalloc(infiles[i].shdrs, sh_bytes);

        if (!infiles[i].shdrs)
            die("Unable to allocate memory");

        fseek(infiles[i].fp, infiles[i].ehdr.e_shoff, SEEK_SET);

        if (fread(infiles[i].shdrs, 1, sh_bytes, infiles[i].fp) != sh_bytes)
            msg("Unable to read section headers from file '%s'", paths[i]);

        if (infiles[i].ehdr.e_shstrndx >= SHN_LORESERVE)
            die("unhandeled code path");
            /* TODO handle first entry of sh for shnum here */

        msg("STRING INDEX: %d", infiles[i].ehdr.e_shstrndx);
        infiles[i].secstrtab =
            infiles[i].shdrs[infiles[i].ehdr.e_shstrndx];

        xmalloc(infiles[i].secstr, infiles[i].secstrtab.sh_size);
        fseek(infiles[i].fp, infiles[i].secstrtab.sh_offset, SEEK_SET);
        xfread(infiles[i].secstr, infiles[i].secstrtab.sh_size, infiles[i].fp);

        infiles[i].symstr = 0;
        for (int n = SHN_UNDEF + 1; n < infiles[i].ehdr.e_shnum; n++) {
            msg("Name: %s Type: %s",
                    &infiles[i].secstr[infiles[i].shdrs[n].sh_name],
                    shdrtype_str(infiles[i].shdrs[n].sh_type));
            if (!strcmp(&infiles[i].secstr[infiles[i].shdrs[n].sh_name],
                        ".strtab")) {
                infiles[i].symstrtab = infiles[i].shdrs[n];
                xmalloc(infiles[i].symstr, infiles[i].symstrtab.sh_size);
                fseek(infiles[i].fp, infiles[i].symstrtab.sh_offset, SEEK_SET);
                xfread(infiles[i].symstr, infiles[i].symstrtab.sh_size, infiles[i].fp);
            }
        }

        if (!infiles[i].symstr)
            die("file has no symbol string table '%s'", paths[i]);

        msg("Section string table contents");
        fwrite(infiles[i].secstr, 1, infiles[i].secstrtab.sh_size, stdout);
        fwrite("\n", 1, 1, stdout);

        msg("Symbol string table contents");
        fwrite(infiles[i].symstr, 1, infiles[i].symstrtab.sh_size, stdout);
        fwrite("\n", 1, 1, stdout);
    }
}

void check_collisions() {
    for (int i = 0; i < infilecnt; i++) {
        msg("allocating symbols");
        /* find symbol tables */
        size_t symbytes = 0;
        size_t relbytes = 0;
        size_t relabytes = 0;

        for (int n = SHN_UNDEF + 1; n < infiles[i].ehdr.e_shnum; n++) {
            switch (infiles[i].shdrs[n].sh_type) {
                case SHT_SYMTAB:
                    symbytes += infiles[i].shdrs[n].sh_size; 
                    break;
                case SHT_REL:
                    relbytes += infiles[i].shdrs[n].sh_size; 
                    break;
                case SHT_RELA:
                    relabytes += infiles[i].shdrs[n].sh_size; 
                    break;
            }
        }

        xmalloc(infiles[i].syms, symbytes);
        xmalloc(infiles[i].rels, relbytes);
        xmalloc(infiles[i].relas, relabytes);
        infiles[i].symcnt = symbytes / sizeof(Elf64_Sym);
        infiles[i].relcnt = relbytes / sizeof(Elf64_Rel);
        infiles[i].relacnt = relabytes / sizeof(Elf64_Rela);
        size_t symcnt = 0;
        size_t relcnt = 0;
        size_t relacnt = 0;

        msg("reading symbols");
        for (int n = 0; n < infiles[i].ehdr.e_shnum; n++) {
            switch (infiles[i].shdrs[n].sh_type) {
                case SHT_SYMTAB:
                    fseek(infiles[i].fp, infiles[i].shdrs[n].sh_offset, SEEK_SET);
                    xfread(&infiles[i].syms[symcnt], infiles[i].shdrs[n].sh_size,
                            infiles[i].fp);
                    symcnt++;
                    break;
                case SHT_REL:
                    fseek(infiles[i].fp, infiles[i].shdrs[n].sh_offset, SEEK_SET);
                    xfread(&infiles[i].rels[relcnt], infiles[i].shdrs[n].sh_size,
                            infiles[i].fp);
                    relcnt++;
                    break;
                case SHT_RELA:
                    fseek(infiles[i].fp, infiles[i].shdrs[n].sh_offset, SEEK_SET);
                    xfread(&infiles[i].relas[relacnt], infiles[i].shdrs[n].sh_size,
                            infiles[i].fp);
                    relacnt++;
                    break;
            }
        }

        if (symbytes / sizeof(Elf64_Sym) != infiles[i].symcnt)
            die("Wrong Size");
        if (relbytes / sizeof(Elf64_Rel) != infiles[i].relcnt)
            die("Wrong Size");
        if (relabytes / sizeof(Elf64_Rela) != infiles[i].relacnt)
            die("Wrong Size");

        /* TODO remove all debugging prints */
        msg("printing syms");
        for (int n = STN_UNDEF + 1; n < infiles[i].symcnt; n++) {
            puts(&infiles[i].symstr[infiles[i].syms[n].st_name]);
            if (infiles[i].syms[n].st_shndx == SHN_UNDEF)
                msg("UNDEF");
        }

        msg("printing rels");
        for (int n = 0; n < infiles[i].relcnt; n++) {
            printf("name: %s, offset: %ld\n", &infiles[i].symstr[ELF64_R_SYM(infiles[i].rels[n].r_info)], infiles[i].rels[n].r_offset);
        }

        msg("printing relas");
        for (int n = 0; n < infiles[i].relacnt; n++) {
            printf("name: %s, offset: %ld addend: %ld\n", &infiles[i].symstr[infiles[i].syms[ELF64_R_SYM(infiles[i].relas[n].r_info)].st_name], infiles[i].relas[n].r_offset, infiles[i].relas[n].r_addend);
        }

    }

    /* TODO compare symbols (collisions) */

    msg("CHECKING COLLISIONS");

    /* TODO use a hashmap here instead of expensive comparissons */

    size_t symcnt = 0;
    for (int i = 0; i < infilecnt; i++) {
        symcnt += infiles[i].symcnt;
    }

    /* TODO, store symbol indecies for each sym_def */
    sym_def* syms;
    /* assume all symbols are unique to make sure there is enough */
    /* memory */
    xmalloc(syms, symcnt * sizeof(sym_def));

    symcnt = 0;

    for (int i = 0; i < infilecnt; i++) {
        for (int n = STN_UNDEF + 1; n < infiles[i].symcnt; n++) {
            uint8_t new = 1;
            for (int k = 0; k < symcnt; k++) {
                /* if symbol is already defined */
                /* TODO check symbol type and collisions */
                if (!strcmp(syms[k].name,
                            &infiles[i].symstr[infiles[i].syms[n].st_name])) {

                    syms[k].defs += SYM_IS_DEF(infiles[i].syms[n]);

                    syms[k].symcnt++;
                    xrealloc(syms[k].syms, sizeof(Elf64_Sym*) * syms[k].symcnt);
                    syms[k].syms[symcnt - 1] = &infiles[i].syms[n];

                    new = 0;
                }
            }
            if (new) {
                syms[symcnt].name =
                    &infiles[i].symstr[infiles[i].syms[n].st_name];
                syms[symcnt].defs = SYM_IS_DEF(infiles[i].syms[n]);

                syms[symcnt].symcnt = 1;
                xmalloc(syms[symcnt].syms, sizeof(Elf64_Sym*));
                syms[symcnt].syms[0] = &infiles[i].syms[n];

                symcnt++;

            }
        }
    }

    /* free unneeded memory */
    xrealloc(syms, symcnt * sizeof(sym_def));

    /* check collisions and stuff */
    for (int n = 0; n < symcnt; n++) {
        /* TODO on release, kill program here */
        if (!syms[n].defs)
            msg("UNDEFINED SYMBOL: '%s'", syms[n].name);

        if (syms[n].defs > 1) {
            /* TODO look for the types and check for collisions here */
            for (int k = 0; k < syms[n].symcnt; k++) {
            msg("REDEFINED SYMBOL: '%s', DEFINED %d TIMES", syms[n].name, syms[n].defs);
            }
        }
    }
}

void create_exec(char* filename) {
    FILE* outfp = fopen(filename, "wb");

    if (!outfp)
        die("Unable to start writing to file '%s'", filename);

    Elf64_Phdr = phdr;

    /* TODO move sections, create program header */
}
