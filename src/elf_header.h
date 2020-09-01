#ifndef __ELF_HEADER_H
#define __ELF_HEADER_H
#include <stdio.h>
#include <string.h>

#include "int.h"
#include "error.h"

typedef struct {
    u32 magic;
    u8  bits;
    u8  endian;
    u8  header_version;
    u8  os_abi;
    u64 padding;
    u16 type;
    u16 arch;
    u32 elf_version;
    u32 entry_pos;
    u32 program_header_table_pos;
    u32 section_header_table_pos;
    u32 flags;
    u16 header_size;
    u16 program_header_table_size;
    u16 program_header_table_count;
    u16 section_header_table_size;
    u16 section_header_table_count;
    u16 section_header_name_index;
} elf32_header;

typedef struct {
    u32 magic;
    u8  bits;
    u8  endian;
    u8  header_version;
    u8  os_abi;
    u64 padding;
    u16 type;
    u16 arch;
    u32 elf_version;
    u64 entry_pos;
    u64 program_header_table_pos;
    u64 section_header_table_pos;
    u32 flags;
    u16 header_size;
    u16 program_header_table_size;
    u16 program_header_table_count;
    u16 section_header_table_size;
    u16 section_header_table_count;
    u16 section_header_name_index;
} elf64_header;

typedef elf64_header elf_header;

void elf64_header_read(elf64_header* h, u8* b);

void elf32_header_read(elf32_header* h, u8* b);

error elf64_header_read_at(elf64_header* h, FILE* f);

error elf32_header_read_at(elf32_header* h, FILE* f);

#endif
