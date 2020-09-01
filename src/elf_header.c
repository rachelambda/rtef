#include "elf_header.h"

void elf64_header_read(elf64_header* h, u8* b) {
    memcpy(h, b, sizeof(elf64_header));
}

void elf32_header_read(elf32_header* h, u8* b) {
    memcpy(h, b, sizeof(elf32_header));
}

error elf64_header_read_at(elf64_header* h, FILE* f) {
    if (fread(h, sizeof(elf64_header), 1, f) != sizeof(elf64_header))
        return ERROR_FREAD;
    return ERROR_NO_ERROR;
}

error elf32_header_read_at(elf32_header* h, FILE* f) {
    if (fread(h, sizeof(elf32_header), 1, f) != sizeof(elf32_header))
        return ERROR_FREAD;
    return ERROR_NO_ERROR;
}
