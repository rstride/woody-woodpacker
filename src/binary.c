/* binary.c */
#include "woody.h"
#include <string.h>

/* Add the injection code, parameters, and key to the destination */
void add_injection(t_elf *elf, void **dst, Elf64_Addr new_entry, t_key *key) {
    size_t inject_header_size = INJECT_SIZE - (sizeof(uint64_t) * 7) + 5;
    memcpy(*dst, INJECT, inject_header_size);
    *dst = (char *)(*dst) + inject_header_size;

    uint64_t *params = (uint64_t *)(*dst);
    params[0] = elf->pt_load->p_vaddr;
    params[1] = elf->pt_load->p_offset;
    params[2] = elf->text_section->sh_offset;
    params[3] = elf->text_section->sh_size;
    params[4] = new_entry;
    params[5] = elf->header->e_entry; // old_entry
    params[6] = key->size;
    *dst = (char *)(*dst) + sizeof(uint64_t) * 7;

    memcpy(*dst, INJECT + (INJECT_SIZE - 5), 5);
    *dst = (char *)(*dst) + 5;
    memcpy(*dst, key->str, key->size);
    *dst = (char *)(*dst) + key->size;
}

/* Fill the encrypted text section into the destination */
int fill_encrypted_text(t_elf *elf, void *src, void **dst, t_key *key) {
    char *encrypt = xor_encrypt((char *)elf->addr + elf->text_section->sh_offset,
                                elf->text_section->sh_size, key);
    if (!encrypt)
        return MALLOC_ERROR;

    size_t copy_size = elf->text_section->sh_offset - ((char *)src - (char *)elf->addr);
    memcpy(*dst, src, copy_size);
    *dst = (char *)(*dst) + copy_size;
    src = (char *)src + copy_size;

    memcpy(*dst, encrypt, elf->text_section->sh_size);
    *dst = (char *)(*dst) + elf->text_section->sh_size;
    src = (char *)src + elf->text_section->sh_size;
    free(encrypt);

    return 0;
}

/* Fill the ELF header and update the entry point */
void *fill_header(t_elf *elf, void *src, void **dst, Elf64_Addr new_entry) {
    size_t offset = (char *)&elf->header->e_entry - (char *)src;
    memcpy(*dst, src, offset);
    *dst = (char *)(*dst) + offset;
    src = (char *)src + offset;

    memcpy(*dst, &new_entry, sizeof(new_entry));
    *dst = (char *)(*dst) + sizeof(new_entry);
    src = (char *)src + sizeof(elf->header->e_entry);

    return src;
}

/* Fill the destination with the modified ELF content */
int fill_binary(t_elf *elf, t_key *key, void *dst, int type) {
    void *src = elf->addr;
    Elf64_Addr new_entry = elf->pt_load->p_vaddr + elf->pt_load->p_memsz;
    int ret;

    src = fill_header(elf, src, &dst, new_entry);

    if (type == ADD_PADDING)
        src = add_padding_segments(elf, src, &dst, key);
    else
        src = update_segment_sz(src, &dst, elf->pt_load, key);

    ret = fill_encrypted_text(elf, src, &dst, key);
    if (ret)
        return ret;

    src = (char *)elf->addr + elf->text_section->sh_offset + elf->text_section->sh_size;
    size_t copy_size = (elf->pt_load->p_offset + elf->pt_load->p_filesz) - ((char *)src - (char *)elf->addr);
    memcpy(dst, src, copy_size);
    dst = (char *)dst + copy_size;
    src = (char *)src + copy_size;

    add_injection(elf, &dst, new_entry, key);

    if (type == ADD_PADDING)
        src = add_padding_sections(elf, src, &dst, key);
    else
        src = (char *)src + INJECT_SIZE + key->size;

    size_t remaining_size = elf->size - ((char *)src - (char *)elf->addr);
    memcpy(dst, src, remaining_size);

    return 0;
}
