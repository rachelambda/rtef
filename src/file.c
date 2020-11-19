#include <string.h>
#include <stdio.h>
#include <elf.h>

#include "global.h"
#include "file.h"

#define START_IDENT "_start"
/* remove if not unix, is only used to mark file executable */
#define UNIX

#ifdef UNIX
#include <sys/stat.h>
#endif

/*
NOTE most of the todos here are things that should be done
once the program is working in order to make it faster,
less memory intensive etc, and will not be done until the
program actually runs. once the program has achieved basic
functionality these will be resolved in order to make the
program good.
*/

/* TODO once working look for places to free memory */

/* TODO look for places to replace multiple for loops with a single one */

elf_file* infiles;
size_t infilecnt;

/* TODO replace with hashtable based on name */
sym_def* syms;
size_t symcnt;

sec_def* secs;
size_t seccnt;

uint8_t has_entry = 0;
Elf64_Addr entry_point = 0;

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

        has_entry += infiles[i].ehdr.e_entry;
        if (has_entry > 1)
            die("Multiple entry points");

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

        xmalloc(infiles[i].relsecs, infiles[i].relcnt * sizeof(char*));
        xmalloc(infiles[i].relasecs, infiles[i].relacnt * sizeof(char*));

        symcnt = 0;
        size_t relcnt = 0;
        size_t relacnt = 0;

        msg("reading symbols");
        for (int n = 0; n < infiles[i].ehdr.e_shnum; n++) {
            switch (infiles[i].shdrs[n].sh_type) {
                case SHT_SYMTAB:
                    fseek(infiles[i].fp, infiles[i].shdrs[n].sh_offset, SEEK_SET);
                    xfread(&infiles[i].syms[symcnt], infiles[i].shdrs[n].sh_size,
                            infiles[i].fp);
                    symcnt += infiles[i].shdrs[n].sh_size / sizeof(Elf64_Sym);
                    break;
                case SHT_REL:
                    fseek(infiles[i].fp, infiles[i].shdrs[n].sh_offset, SEEK_SET);
                    xfread(&infiles[i].rels[relcnt], infiles[i].shdrs[n].sh_size,
                            infiles[i].fp);
                    size_t old_relcnt = relcnt;
                    relcnt += infiles[i].shdrs[n].sh_size / sizeof(Elf64_Rel);
                    char* relsec = strrchr(&infiles[i].secstr[infiles[i].shdrs[n].sh_name],
                            '.');
                    msg("RELSEC: '%s'", relsec);
                    for (int k = old_relcnt; k < infiles[i].relcnt; k++)
                        infiles[i].relsecs[k] = relsec;
                    break;
                case SHT_RELA:
                    fseek(infiles[i].fp, infiles[i].shdrs[n].sh_offset, SEEK_SET);
                    xfread(&infiles[i].relas[relacnt], infiles[i].shdrs[n].sh_size,
                            infiles[i].fp);
                    size_t old_relacnt = relacnt;
                    relacnt += infiles[i].shdrs[n].sh_size / sizeof(Elf64_Rela);
                    char* relasec = strrchr(&infiles[i].secstr[infiles[i].shdrs[n].sh_name],
                            '.');
                    msg("RELASEC: '%s'", relasec);
                    for (int k = old_relacnt; k < infiles[i].relacnt; k++)
                        infiles[i].relasecs[k] = relasec;
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
            msg(&infiles[i].symstr[infiles[i].syms[n].st_name]);
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

    symcnt = 0;
    for (int i = 0; i < infilecnt; i++) {
        symcnt += infiles[i].symcnt;
    }

    /* assume all symbols are unique to make sure there is enough */
    /* memory */
    xmalloc(syms, symcnt * sizeof(sym_def));

    symcnt = 0;

    for (int i = 0; i < infilecnt; i++) {
        for (int n = STN_UNDEF + 1; n < infiles[i].symcnt; n++) {
            uint8_t new = 1;
            for (int k = 0; k < symcnt; k++) {
                /* if symbol is already defined */
                if (!strcmp(syms[k].name,
                            &infiles[i].symstr[infiles[i].syms[n].st_name])) {

                    syms[k].defs += SYM_IS_DEF(infiles[i].syms[n]);

                    syms[k].symcnt++;
                    xrealloc(syms[k].syms, sizeof(Elf64_Sym*) * syms[k].symcnt);
                    syms[k].syms[syms[k].symcnt - 1] = &infiles[i].syms[n];

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

    /* organise section headers */
    seccnt = 0;
    for (int i = 0; i < infilecnt; i++) {
        seccnt += infiles[i].ehdr.e_shnum;
    }

    /* assume all sections are unique to make sure there is enough */
    /* memory */
    xmalloc(secs, seccnt * sizeof(sec_def));

    seccnt = 0;

    /* TODO add support for other sections */
    for (int i = 0; i < infilecnt; i++) {
        xmalloc(infiles[i].sec_indecies, infiles[i].ehdr.e_shnum * sizeof(size_t));
        xmalloc(infiles[i].sec_defs, infiles[i].ehdr.e_shnum * sizeof(sec_def*));
        for (int n = SHN_UNDEF + 1; n < infiles[i].ehdr.e_shnum; n++) {
            if(infiles[i].shdrs[n].sh_type != SHT_NOBITS
                    && infiles[i].shdrs[n].sh_type != SHT_PROGBITS)
                continue;
            uint8_t new = 1;
            for (int k = 0; k < seccnt; k++) {
                /* if section is already defined */
                /* since we can only have .text, .bss or .data here */
                /* only one char needs to be compared */
                if (!strcmp(secs[k].name,
                            &infiles[i].secstr[infiles[i].shdrs[n].sh_name])) {

                    secs[k].seccnt++;

                    if (infiles[i].shdrs[n].sh_flags & SHF_WRITE)
                        secs[k].p_flags |= PF_W;
                    if (infiles[i].shdrs[n].sh_flags & SHF_EXECINSTR)
                        secs[k].p_flags |= PF_X;

                    xrealloc(secs[k].secs, sizeof(*secs[k].secs) * secs[k].seccnt);
                    secs[k].secs[secs[k].seccnt - 1].sec = &infiles[i].shdrs[n];
                    secs[k].secs[secs[k].seccnt - 1].file = &infiles[i];

                    infiles[i].sec_defs[n] = &secs[k];
                    infiles[i].sec_indecies[n] = secs[k].seccnt - 1;

                    new = 0;
                }
            }
            if (new) {
                secs[seccnt].name =
                    &infiles[i].secstr[infiles[i].shdrs[n].sh_name];

                secs[seccnt].seccnt = 1;

                /* for now assume all sections can be read, possibly add an option */
                /* to disable this for specific sections in case you're working on */
                /* firmware for example */
                secs[seccnt].p_flags = PF_R;
                if (infiles[i].shdrs[n].sh_flags & SHF_WRITE)
                    secs[seccnt].p_flags |= PF_W;
                if (infiles[i].shdrs[n].sh_flags & SHF_EXECINSTR)
                    secs[seccnt].p_flags |= PF_X;

                xmalloc(secs[seccnt].secs, sizeof(*secs[seccnt].secs));
                secs[seccnt].secs[0].sec = &infiles[i].shdrs[n];
                secs[seccnt].secs[0].file = &infiles[i];

                infiles[i].sec_defs[n] = &secs[seccnt];
                infiles[i].sec_indecies[n] = 0;

                seccnt++;


            }
        }
    }

    /* free unneeded memory */
    xrealloc(secs, seccnt * sizeof(sec_def));

    /* output final list of section headers */
    for (int n = 0; n < seccnt; n++) {
        msg("section %s", secs[n].name);
    }
}

void create_exec(char* filename) {
    msg("WRITING FILE");

    /* calculate sub section offsets to merge sections */
    Elf64_Addr addr_counter = 0x20000000; /* TODO better way of handelin addrs */
    for (int n = 0; n < seccnt; n++) {
        msg("calculating alignment and offset for section: '%s'", secs[n].name);
        /* find largest alignment */
        size_t alignment_index = 0;
        for (int i = 0; i < secs[n].seccnt; i++) {
            if (secs[n].secs[i].sec->sh_addralign >
                    secs[n].secs[alignment_index].sec->sh_addralign)
                alignment_index = i;
        }

        if (secs[n].secs[alignment_index].sec->sh_addralign < 0x1000)
            secs[n].secs[alignment_index].sec->sh_addralign = 0x1000;
        /* align section to alignment of largest alignment */
        secs[n].align = secs[n].secs[alignment_index].sec->sh_addralign;
        msg("secs align: 0x%x", secs[n].align);
        secs[n].secs[alignment_index].offset = 0;
        secs[n].memory_size = secs[n].secs[alignment_index].sec->sh_size;

        for (int i = 0; i < secs[n].seccnt; i++) {
            if (i == alignment_index)
                continue;
            secs[n].secs[i].offset = secs[n].memory_size + secs[n].secs[i].sec->sh_addralign
                - (secs[n].memory_size % secs[n].secs[i].sec->sh_addralign);
            secs[n].memory_size = secs[n].secs[i].offset + secs[n].secs[i].sec->sh_size;
        }

        for (int i = 0; i < secs[n].seccnt; i++) {
            if (secs[n].secs[i].offset &&
                    secs[n].secs[i].sec->sh_addr % secs[n].secs[i].offset)
                die("failed alignment");
        }

        /* TODO allow setting custom addr here */
        secs[n].addr = addr_counter + secs[n].align - (addr_counter % secs[n].align);
        addr_counter = secs[n].addr + secs[n].memory_size;
    }

    /* allocate memory for each section */
    /* NOTE doesn't do special stuff for BSS sections */
    msg("allocating memory for sections");
    for (int n = 0; n < seccnt; n++) {
        xmalloc(secs[n].memory, secs[n].memory_size);
    }

    /* read from file to memory */
    msg("reading sections into memory");
    for (int n = 0; n < seccnt; n++) {
        for (int i = 0; i < secs[n].seccnt; i++) {
            fseek(secs[n].secs[i].file->fp,
                    secs[n].secs[i].sec->sh_offset,
                    SEEK_SET);
            xfread(secs[n].memory + secs[n].secs[i].offset,
                    secs[n].secs[i].sec->sh_size,
                    secs[n].secs[i].file->fp);
        }
    }

    /* TODO resolve symbols and fill out relocation entries */
    /* TODO make rela entries for each section (possible with hash table) */
    msg("performing relocations");
    for (int n = 0; n < seccnt; n++) {
        for (int i = 0; i < secs[n].seccnt; i++) {
            msg("relocations for section '%s'", secs[n].name);
            for (int k = 0; k < secs[n].secs[i].file->relacnt; k++) {
                if (!strcmp(secs[n].secs[i].file->relasecs[k], secs[n].name))
                        msg("matching relocation file"); else continue;
                Elf64_Addr value;
                elf_file* defined_file;
                Elf64_Sym* defined_sym;
                Elf64_Rela* rela = &secs[n].secs[i].file->relas[k];
                uint8_t* addr = (secs[n].memory + secs[n].secs[i].offset
                 + rela->r_offset);
                char* symname =
                    &secs[n].secs[i].file->symstr[
                    secs[n].secs[i].file->syms[ELF64_R_SYM(rela->r_info)].st_name];

                char* secname =
                    &secs[n].secs[i].file->secstr[secs[n].secs[i].file->shdrs[
                    secs[n].secs[i].file->syms[ELF64_R_SYM(rela->r_info)].st_shndx].sh_name];

                for (int x = 0; x < infilecnt; x++) {
                    for (int y = 0; y < infiles[x].symcnt; y++) {
                        if (SYM_IS_DEF(infiles[x].syms[y]) &&
                                !strcmp(&infiles[x].symstr[infiles[x].syms[y].st_name], symname)) {
                            size_t sec_index = infiles[x].syms[y].st_shndx;
                            Elf64_Addr st_val = infiles[x].syms[y].st_value;
                            Elf64_Addr addr = sec_index == SHN_ABS ? 0 :
                                infiles[x].sec_defs[sec_index]->addr;
                            Elf64_Addr offset = sec_index == SHN_ABS ? 0 :
                                infiles[x].sec_defs[sec_index]->secs[
                                        infiles[x].sec_indecies[sec_index]].offset;
                            if (ELF64_ST_TYPE(infiles[x].syms[y].st_info) == STT_SECTION) {
                                /* compare names of sections */
                                msg("defined relative to %s", infiles[x].sec_defs[sec_index]->name);
                                if (strcmp(&infiles[x].secstr[infiles[x].shdrs[sec_index].sh_name],
                                            secname))
                                        continue;

                                /* not sure if val is needed here */
                                value = addr + offset + st_val;

                                /* set x to break both loops */
                                x = infilecnt;
                                break;

                            } else if (infiles[x].syms[y].st_shndx == SHN_ABS) {
                                /* just add the value */
                                value = st_val;
                            } else {
                                /* relocate based on section offsets */
                                value = st_val + addr + offset;
                            }
                        }
                    }
                }

                uint64_t final_val;
                uint8_t val_size;

                switch (ELF64_R_TYPE(rela->r_info)) {
                    case R_X86_64_NONE:
                        msg("NONE relocation type... WHY???");
                        break;
                    case R_X86_64_32S:
                        val_size = 64;
                        final_val = ((uint32_t)value >> 31) ? ((uint32_t)value | (uint64_t)0xffffffff00000000) + rela->r_addend :
                            (uint32_t)value + rela->r_addend;
                        break;
                    case R_X86_64_32:
                    case R_X86_64_64:
                        msg("R_X86_64_64");
                        val_size = 64;
                        final_val = value + rela->r_addend;
                        break;
                    case R_X86_64_PC32:
                        val_size = 32;
                        msg("R_X86_64_PC32, 0x%lx, %ld, 0x%lx", value, rela->r_addend, (rela->r_offset + secs[n].addr + secs[n].secs[i].offset));
                        final_val = value + rela->r_addend - (rela->r_offset + secs[n].addr + secs[n].secs[i].offset);
                        break;
                    default:
                        die("Unsupported relocation type");
                }

                msg("relocating '%s' of size %lu to 0x%lx", symname, val_size, final_val);
                msg("addr: 0x%lx", *(uint64_t*)addr);
                switch (val_size) {
                    case 8:
                        *(uint8_t*)addr = final_val;
                        break;
                    case 16:
                        *(uint16_t*)addr = final_val;
                        break;
                    case 32:
                        *(uint32_t*)addr = final_val;
                        break;
                    case 64:
                        *(uint64_t*)addr = final_val;
                        break;
                }
                msg("addr: 0x%lx", *(uint64_t*)addr);
            }
        }
    }

    /* TODO tidy up this */
    msg("finding start point");
    char* symname = START_IDENT;
    for (int x = 0; x < infilecnt; x++) {
        int match = -1;
        for (int y = 0; y < infiles[x].symcnt; y++) {
            if (!strcmp(&infiles[x].symstr[infiles[x].syms[y].st_name], symname))
                    match = y;
        }
        if (match != -1 && infiles[x].syms[match].st_shndx != SHN_UNDEF) {
            elf_file* defined_file = &infiles[x];
            Elf64_Sym* defined_sym = &infiles[x].syms[match];
            if (defined_sym->st_shndx != SHN_ABS) {
                uint8_t found = 0;
                for (int y = 0; y < seccnt; y++) {
                    for (int s = 0; s < secs[y].seccnt; s++) {
                        if (secs[y].secs[s].file == defined_file &&
                                secs[y].secs[s].sec ==
                                &defined_file->shdrs[defined_sym->st_shndx]
                                ) {
                            msg("adding 0x%lx to value %lx of sym '%s'",
                                    secs[y].secs[s].offset,
                                    defined_sym->st_value +
                                    secs[y].addr,
                                    symname);
                            found = 1;
                            defined_sym->st_value += secs[y].secs[s].offset + secs[y].addr;
                            entry_point = defined_sym->st_value;
                            /* find entry point */
                            break;
                        }
                    }
                }
                if (!found)
                    die("symbol '%s' lacks section definition", symname);
            }
        }
    }

    msg("creating program headers");
    Elf64_Ehdr ehdr;

    ehdr.e_ident[EI_MAG0] = 0x7f;
    ehdr.e_ident[EI_MAG1] = 'E';
    ehdr.e_ident[EI_MAG2] = 'L';
    ehdr.e_ident[EI_MAG3] = 'F';
    ehdr.e_ident[EI_CLASS] = ELFCLASS64;
    ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    ehdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
    ehdr.e_ident[EI_ABIVERSION] = 0;

    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_X86_64;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_entry = entry_point;
    ehdr.e_phoff = sizeof(Elf64_Ehdr);
    ehdr.e_shoff = 0;
    ehdr.e_flags = 0;
    ehdr.e_ehsize = sizeof(Elf64_Ehdr);
    ehdr.e_phentsize = sizeof(Elf64_Phdr);
    ehdr.e_phnum = seccnt; /* might change */
    ehdr.e_shentsize = sizeof(Elf64_Shdr);
    ehdr.e_shnum = 0;
    ehdr.e_shstrndx = SHN_UNDEF;

    Elf64_Phdr phdrs[seccnt];

    size_t file_offset = sizeof(Elf64_Ehdr) + (sizeof(Elf64_Phdr) * seccnt);

    for (int n = 0; n < seccnt; n++) {
        /* TODO replace with */ 
        /*     dq 0x400000 + _start    ;   p_vaddr */
        /* method to save space */
        secs[n].offset = file_offset + secs[n].align - (file_offset % secs[n].align);
        file_offset = secs[n].offset + secs[n].memory_size;
        phdrs[n].p_type = PT_LOAD;
        phdrs[n].p_offset = secs[n].offset;
        phdrs[n].p_offset = secs[n].offset;
        phdrs[n].p_vaddr = secs[n].addr;
        phdrs[n].p_paddr = 0;
        phdrs[n].p_align = secs[n].align;
        phdrs[n].p_flags = secs[n].p_flags;
        phdrs[n].p_filesz = secs[n].memory_size;
        phdrs[n].p_memsz = secs[n].memory_size;
        msg("section '%s' will be loaded at 0x%lx", secs[n].name, phdrs[n].p_vaddr);
    }

    FILE* outfp = fopen(filename, "wb");

    if (!outfp)
        die("Unable to start writing to file '%s'", filename);

    fwrite(&ehdr, 1, sizeof(Elf64_Ehdr), outfp);
    fwrite(phdrs, 1, sizeof(Elf64_Phdr) * seccnt, outfp);
    for (int n = 0; n < seccnt; n++) {
        /* shouldn't be neccessary */
        fseek(outfp, secs[n].offset, SEEK_SET);
        fwrite(secs[n].memory, 1, secs[n].memory_size, outfp);
    }

#ifdef UNIX
    chmod(filename, S_IRUSR | S_IWUSR | S_IXUSR |
            S_IRGRP | S_IWGRP | S_IXGRP |
            S_IROTH | S_IWOTH | S_IXOTH );
#endif

    fclose(outfp);
}
