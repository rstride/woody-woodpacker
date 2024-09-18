#include "woody.h"

/**
 * @brief Injects the payload, parameters, and key into the destination buffer.
 *
 * @param elf ELF structure.
 * @param dest Destination buffer pointer.
 * @param new_entry New entry point for the modified file.
 * @param key Generated key.
 */
void inject_payload(t_elf *elf, void **dest, Elf64_Addr new_entry, t_key *key) {
    ft_memcpy(*dest, INJECT, INJECT_SIZE - (sizeof(uint64_t) * 7) + 5);
    *dest += INJECT_SIZE - ((sizeof(uint64_t) * 7) + 5);
    ft_memcpy(*dest + sizeof(uint64_t) * 0, &elf->pt_load->p_vaddr, sizeof(uint64_t));
    ft_memcpy(*dest + sizeof(uint64_t) * 1, &elf->pt_load->p_offset, sizeof(uint64_t));
    ft_memcpy(*dest + sizeof(uint64_t) * 2, &elf->text_section->sh_offset, sizeof(uint64_t));
    ft_memcpy(*dest + sizeof(uint64_t) * 3, &elf->text_section->sh_size, sizeof(uint64_t));
    ft_memcpy(*dest + sizeof(uint64_t) * 4, &new_entry, sizeof(uint64_t));
    ft_memcpy(*dest + sizeof(uint64_t) * 5, &elf->header->e_entry, sizeof(uint64_t)); // old_entry
    ft_memcpy(*dest + sizeof(uint64_t) * 6, &key->size, sizeof(uint64_t));
    *dest += sizeof(uint64_t) * 7;
    ft_memcpy(*dest, INJECT + (INJECT_SIZE - 5), 5);
    *dest += 5;
    ft_memcpy(*dest, key->str, key->size);
    *dest += key->size;
}

/**
 * @brief Encrypts the text section and copies it to the destination buffer.
 *
 * @param elf ELF structure.
 * @param src Source buffer pointer.
 * @param dest Destination buffer pointer.
 * @param key Generated key.
 *
 * @return 0 on success, error code on failure.
 */
int encrypt_text_section(t_elf *elf, void *src, void **dest, t_key *key) {
    char *encrypted_text;

    encrypted_text = xor_encrypt(elf->addr + elf->text_section->sh_offset, elf->text_section->sh_size, key);
    if (!encrypted_text)
        return MALLOC_ERROR;

    ft_memcpy(*dest, src, ((unsigned long)elf->addr + (unsigned long)elf->text_section->sh_offset) - (unsigned long)src);
    *dest += ((unsigned long)elf->addr + (unsigned long)elf->text_section->sh_offset) - (unsigned long)src;
    ft_memcpy(*dest, encrypted_text, elf->text_section->sh_size);
    *dest += elf->text_section->sh_size;
    free(encrypted_text);

    return 0;
}

/**
 * @brief Updates the ELF header with the new entry point.
 *
 * @param elf ELF structure.
 * @param src Source buffer pointer.
 * @param dest Destination buffer pointer.
 * @param new_entry New entry point for the modified file.
 *
 * @return Updated source buffer pointer.
 */
void *update_elf_header(t_elf *elf, void *src, void **dest, Elf64_Addr new_entry) {
    ft_memcpy(*dest, src, (unsigned long)&elf->header->e_entry - (unsigned long)src);
    *dest += (unsigned long)&elf->header->e_entry - (unsigned long)src;
    ft_memcpy(*dest, &new_entry, sizeof(new_entry));
    *dest += sizeof(new_entry);
    src = (void *)&elf->header->e_entry + sizeof(elf->header->e_entry);

    return src;
}

/**
 * @brief Fills the destination buffer with the ELF header, encrypted text section, payload, padding, and remaining content.
 *
 * @param elf ELF structure.
 * @param key Generated key.
 * @param dest Destination buffer pointer.
 * @param add_padding Flag to indicate whether to add padding.
 *
 * @return 0 on success, error code on failure.
 */
int fill_elf_binary(t_elf *elf, t_key *key, void *dest, int add_padding) {
    int ret;
    void *src;
    void *end;
    Elf64_Addr new_entry;

    src = elf->addr;
    end = elf->addr + elf->size;
    new_entry = elf->pt_load->p_vaddr + elf->pt_load->p_memsz;
    src = update_elf_header(elf, src, &dest, new_entry);

    if (add_padding == ADD_PADDING)
        src = add_padding_segments(elf, src, &dest, key);
    else
        src = update_segment_sz(src, &dest, elf->pt_load, key);

    ret = encrypt_text_section(elf, src, &dest, key);
    if (ret)
        return ret;

    src = elf->addr + elf->text_section->sh_offset + elf->text_section->sh_size;
    ft_memcpy(dest, src, ((unsigned long)elf->addr + (unsigned long)elf->pt_load->p_offset + elf->pt_load->p_memsz) - (unsigned long)src);
    dest += ((unsigned long)elf->addr + (unsigned long)elf->pt_load->p_offset + elf->pt_load->p_memsz) - (unsigned long)src;
    src = elf->addr + elf->pt_load->p_offset + elf->pt_load->p_memsz;

    inject_payload(elf, &dest, new_entry, key);

    if (add_padding == ADD_PADDING)
        src = add_padding_sections(elf, src, &dest, key);
    else
        src += INJECT_SIZE + key->size;

    ft_memcpy(dest, src, end - src);

    return 0;
}