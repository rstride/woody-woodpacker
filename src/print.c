#include "woody.h"
#include <unistd.h>

/* Print a byte in hexadecimal */
void dec_to_hex(unsigned char c) {
    char hex_digits[] = "0123456789abcdef";
    char hex[2];

    hex[0] = hex_digits[(c >> 4) & 0xF];
    hex[1] = hex_digits[c & 0xF];
    write(STDOUT_FILENO, hex, 2);
}

/* Print 16 bytes of memory in hex and ASCII */
void print_line(char *str, int n) {
    char line[64];
    int idx = 0;
    int i;

    /* Build hex part */
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)str[i];
        char hex_digits[] = "0123456789abcdef";

        line[idx++] = hex_digits[(c >> 4) & 0xF];
        line[idx++] = hex_digits[c & 0xF];

        if (i % 2 == 1 && i != n - 1)
            line[idx++] = ' ';
    }

    /* Add spaces to align ASCII part */
    int expected_hex_len = (16 * 2) + 7; /* Max hex length with spaces */
    while (idx < expected_hex_len)
        line[idx++] = ' ';

    line[idx++] = ' ';

    /* Build ASCII part */
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)str[i];
        line[idx++] = (c >= 32 && c <= 126) ? c : '.';
    }

    line[idx++] = '\n';
    write(STDOUT_FILENO, line, idx);
}

/* Print memory starting from addr of given size */
void *ft_print_memory(void *addr, unsigned int size) {
    char *str = addr;
    unsigned int offset = 0;

    while (offset < size) {
        int n = (size - offset < 16) ? size - offset : 16;
        print_line(&str[offset], n);
        offset += n;
    }
    return addr;
}

/* Print the key in hexadecimal with '\x' prefix */
void print_hexa_key(char *key, size_t size) {
    char buf[256]; /* Ensure this is large enough for your key size */
    size_t idx = 0;

    for (size_t i = 0; i < size; i++) {
        unsigned char c = (unsigned char)key[i];
        char hex_digits[] = "0123456789abcdef";

        buf[idx++] = '\\';
        buf[idx++] = 'x';
        buf[idx++] = hex_digits[(c >> 4) & 0xF];
        buf[idx++] = hex_digits[c & 0xF];
    }
    write(STDOUT_FILENO, buf, idx);
}
