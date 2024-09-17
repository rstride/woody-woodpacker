/* elf.c */
#include "woody.h"
#include <string.h>

/* Get the .text section from the ELF file */
Elf64_Shdr *get_text_section(t_elf *elf) {
    Elf64_Shdr *str_table = NULL;
    char *strtab;

    for (uint16_t i = 0; i < elf->header->e_shnum; i++) {
        if (elf->sections[i].sh_offset > elf->size)
            return NULL;
        if (elf->sections[i].sh_type == SHT_STRTAB && i == elf->header->e_shstrndx)
            str_table = &elf->sections[i];
    }
    if (!str_table)
        return NULL;
    strtab = (char *)elf->addr + str_table->sh_offset;
    for (uint16_t i = 0; i < elf->header->e_shnum; i++) {
        if (strcmp(strtab + elf->sections[i].sh_name, ".text") == 0)
            return &elf->sections[i];
    }
    return NULL;
}

/* Initialize the ELF structure with pointers from addr */
int init_elf(t_elf *elf, void *addr, size_t size) {
    elf->addr = addr;
    elf->size = size;
    elf->header = (Elf64_Ehdr *)addr;

    if (elf->header->e_phoff > size || elf->header->e_shoff > size)
        return CORRUPTED_FILE;

    elf->segments = (Elf64_Phdr *)((char *)addr + elf->header->e_phoff);
    elf->sections = (Elf64_Shdr *)((char *)addr + elf->header->e_shoff);
    elf->text_section = get_text_section(elf);
    if (!elf->text_section)
        return CORRUPTED_FILE;

    elf->pt_load = NULL;
    for (uint16_t i = 0; i < elf->header->e_phnum; i++) {
        if (elf->segments[i].p_type == PT_LOAD && (elf->segments[i].p_flags & PF_X)) {
            elf->pt_load = &elf->segments[i];
            break;
        }
    }
    if (!elf->pt_load)
        return EINVAL;

    return 0;
}

/* Check if the file has the ELF64 magic bytes */
int check_file(void *addr) {
    unsigned char *magic = (unsigned char *)addr;
    Elf64_Ehdr *header = (Elf64_Ehdr *)addr;

    if (magic[EI_MAG0] == ELFMAG0 && magic[EI_MAG1] == ELFMAG1 &&
        magic[EI_MAG2] == ELFMAG2 && magic[EI_MAG3] == ELFMAG3) {
        if (magic[EI_CLASS] == ELFCLASS64)
            return header->e_type;
    }
    return ET_NONE;
}
