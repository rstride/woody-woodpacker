#include "woody.h"
#include <string.h>

/* Update p_filesz and p_memsz of the segment for injection */
void *update_segment_sz(void *src, void **dst, Elf64_Phdr *segment, t_key *key) {
    uint64_t new_p_filesz = segment->p_filesz + INJECT_SIZE + key->size;
    uint64_t new_p_memsz = segment->p_memsz + INJECT_SIZE + key->size;

    size_t offset = (char *)&segment->p_filesz - (char *)src;
    memcpy(*dst, src, offset);
    *dst = (char *)(*dst) + offset;
    src = (char *)src + offset;

    memcpy(*dst, &new_p_filesz, sizeof(new_p_filesz));
    *dst = (char *)(*dst) + sizeof(new_p_filesz);
    src = (char *)src + sizeof(new_p_filesz);

    memcpy(*dst, &new_p_memsz, sizeof(new_p_memsz));
    *dst = (char *)(*dst) + sizeof(new_p_memsz);
    src = (char *)src + sizeof(new_p_memsz);

    return src;
}

/* Add padding to segments after injection */
void *add_padding_segments(t_elf *elf, void *src, void **dst, t_key *key) {
    Elf64_Off new_shoff = elf->header->e_shoff + PAGE_SIZE;

    size_t offset = (char *)&elf->header->e_shoff - (char *)src;
    memcpy(*dst, src, offset);
    *dst = (char *)(*dst) + offset;
    src = (char *)src + offset;

    memcpy(*dst, &new_shoff, sizeof(new_shoff));
    *dst = (char *)(*dst) + sizeof(new_shoff);
    src = (char *)src + sizeof(new_shoff);

    for (int i = 0; i < elf->header->e_phnum; i++) {
        if (&elf->segments[i] == elf->pt_load) {
            src = update_segment_sz(src, dst, elf->pt_load, key);
        } else if (elf->segments[i].p_offset >= elf->pt_load->p_offset + elf->pt_load->p_filesz) {
            /* Update segment offsets and addresses */
            Elf64_Off new_p_offset = elf->segments[i].p_offset + PAGE_SIZE;
            Elf64_Addr new_p_vaddr = elf->segments[i].p_vaddr + PAGE_SIZE;
            Elf64_Addr new_p_paddr = elf->segments[i].p_paddr + PAGE_SIZE;

            offset = (char *)&elf->segments[i].p_offset - (char *)src;
            memcpy(*dst, src, offset);
            *dst = (char *)(*dst) + offset;
            src = (char *)src + offset;

            memcpy(*dst, &new_p_offset, sizeof(new_p_offset));
            *dst = (char *)(*dst) + sizeof(new_p_offset);
            src = (char *)src + sizeof(new_p_offset);

            /* Update p_vaddr */
            offset = (char *)&elf->segments[i].p_vaddr - (char *)src;
            memcpy(*dst, src, offset);
            *dst = (char *)(*dst) + offset;
            src = (char *)src + offset;

            memcpy(*dst, &new_p_vaddr, sizeof(new_p_vaddr));
            *dst = (char *)(*dst) + sizeof(new_p_vaddr);
            src = (char *)src + sizeof(new_p_vaddr);

            /* Update p_paddr */
            offset = (char *)&elf->segments[i].p_paddr - (char *)src;
            memcpy(*dst, src, offset);
            *dst = (char *)(*dst) + offset;
            src = (char *)src + offset;

            memcpy(*dst, &new_p_paddr, sizeof(new_p_paddr));
            *dst = (char *)(*dst) + sizeof(new_p_paddr);
            src = (char *)src + sizeof(new_p_paddr);
        }
    }
    return src;
}

/* Add padding to sections after injection */
void *add_padding_sections(t_elf *elf, void *src, void **dst, t_key *key) {
    Elf64_Phdr *next_segment = elf->pt_load + 1;
    int diff = (INJECT_SIZE + key->size) - (next_segment->p_offset - (elf->pt_load->p_offset + elf->pt_load->p_filesz));
    size_t padding_size = PAGE_SIZE - (diff % PAGE_SIZE);

    memset(*dst, 0, padding_size);
    *dst = (char *)(*dst) + padding_size;

    src = (char *)elf->addr + next_segment->p_offset;

    for (int i = 0; i < elf->header->e_shnum; i++) {
        if (elf->sections[i].sh_offset > elf->pt_load->p_offset + elf->pt_load->p_filesz) {
            /* Update section offsets and addresses */
            Elf64_Off new_sh_offset = elf->sections[i].sh_offset + PAGE_SIZE;
            Elf64_Addr new_sh_addr = elf->sections[i].sh_addr + PAGE_SIZE;

            size_t offset = (char *)&elf->sections[i].sh_offset - (char *)src;
            memcpy(*dst, src, offset);
            *dst = (char *)(*dst) + offset;
            src = (char *)src + offset;

            memcpy(*dst, &new_sh_offset, sizeof(new_sh_offset));
            *dst = (char *)(*dst) + sizeof(new_sh_offset);
            src = (char *)src + sizeof(new_sh_offset);

            /* Update sh_addr */
            offset = (char *)&elf->sections[i].sh_addr - (char *)src;
            memcpy(*dst, src, offset);
            *dst = (char *)(*dst) + offset;
            src = (char *)src + offset;

            memcpy(*dst, &new_sh_addr, sizeof(new_sh_addr));
            *dst = (char *)(*dst) + sizeof(new_sh_addr);
            src = (char *)src + sizeof(new_sh_addr);
        }
    }
    return src;
}
