#include "woody.h"
#include <time.h>

/* Generate a random key of 'size' bytes */
char *generate_key(size_t size) {
    char *key = malloc(size);
    if (!key)
        return NULL;

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    ft_srand(spec.tv_nsec);

    for (size_t i = 0; i < size; i++)
        key[i] = ft_rand() % 256;

    return key;
}

/* Encrypt input using XOR with the provided key */
char *xor_encrypt(char *input, size_t input_len, t_key *key) {
    char *encrypt = malloc(input_len);
    if (!encrypt)
        return NULL;

    for (size_t i = 0; i < input_len; i++)
        encrypt[i] = input[i] ^ key->str[i % key->size];

    return encrypt;
}
