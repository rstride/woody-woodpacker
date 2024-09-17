#include "woody.h"
#include <string.h>
#include <errno.h>

/* Simple linear congruential generator for random numbers */
static unsigned long next_rand = 1;

void ft_srand(unsigned int seed) {
    next_rand = seed;
}

int ft_rand(void) {
    next_rand = next_rand * 1103515245 + 12345;
    return (unsigned int)(next_rand / 65536) % 32768;
}

/* Print error messages based on error codes */
void print_error(char *argv[], int code) {
    const char *error_message;

    switch (code) {
        case CORRUPTED_FILE:
            error_message = "File corrupted";
            break;
        case MALLOC_ERROR:
            error_message = "Malloc error";
            break;
        case OUTPUT_ERROR:
            error_message = strerror(errno);
            break;
        case WRONG_FILETYPE:
            error_message = "Wrong filetype (x86_64 ELF binary only)";
            break;
        case OUT_OF_RANGE:
            error_message = "Cannot resolve this case";
            break;
        default:
            error_message = strerror(errno);
            break;
    }

    fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], error_message);
}

/* Custom implementation of strlen */
size_t ft_strlen(const char *s) {
    const char *ptr = s;
    while (*ptr)
        ptr++;
    return ptr - s;
}

/* Custom implementation of strcmp */
int ft_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/* Custom implementation of memset */
void *ft_memset(void *b, int c, size_t len) {
    unsigned char *ptr = b;
    while (len--)
        *ptr++ = (unsigned char)c;
    return b;
}

/* Custom implementation of memcpy */
void *ft_memcpy(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}
