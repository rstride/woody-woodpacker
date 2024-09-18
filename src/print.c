#include "woody.h"

#define HEX_CHARS "0123456789abcdef"

/**
 * @brief Convert a decimal value to hexadecimal and print it.
 *
 * @param dec The decimal value to convert.
 */
void dec_to_hex(char dec) {
    unsigned char result[2];
    unsigned char copy = (unsigned char)dec;
    int i;

    for (i = 1; i >= 0; i--) {
        result[i] = HEX_CHARS[copy % 16];
        copy /= 16;
    }

    write(STDOUT_FILENO, result, 2);
}

/**
 * @brief Print a line for ft_print_memory.
 *        16 characters as hexadecimal, space every 2 characters,
 *        then print ASCII for those chars. If they are unprintable, print '.'.
 *
 * @param str The string to print.
 * @param n The number of characters to print.
 */
void print_line(char *str, int n) {
    int i;

    if (n == 0) n = 16;

    for (i = 0; i < n; i++) {
        if (i > 0 && i % 2 == 0) write(STDOUT_FILENO, " ", 1);
        dec_to_hex(str[i]);
    }

    for (i = n; i < 16; i++) {
        if (i % 2 == 0) write(STDOUT_FILENO, " ", 1);
        write(STDOUT_FILENO, "  ", 2);
    }

    write(STDOUT_FILENO, " ", 1);

    for (i = 0; i < n; i++) {
        if (str[i] < 32 || str[i] > 126) {
            write(STDOUT_FILENO, ".", 1);
        } else {
            write(STDOUT_FILENO, &str[i], 1);
        }
    }
}

/**
 * @brief Print memory pointed by addr of size 'size'.
 *
 * @param addr The address of the memory to print.
 * @param size The size of the memory to print.
 *
 * @return The address of the memory.
 */
void *ft_print_memory(void *addr, unsigned int size) {
    unsigned int i;
    char *str = (char *)addr;
    char buffer[16];

    for (i = 0; i < size; i++) {
        buffer[i % 16] = str[i];
        if ((i + 1) % 16 == 0 || i == size - 1) {
            print_line(buffer, (i + 1) % 16 == 0 ? 16 : (i % 16) + 1);
            write(STDOUT_FILENO, "\n", 1);
        }
    }

    return addr;
}

/**
 * @brief Print the key in hexadecimal with '\x' before every char.
 *
 * @param key The generated key.
 * @param size The size of the key.
 */
void print_hexa_key(char *key, size_t size) {
    size_t i;

    for (i = 0; i < size; i++) {
        write(STDOUT_FILENO, "\\x", 2);
        dec_to_hex(key[i]);
    }
}
