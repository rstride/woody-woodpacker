#include "woody.h"

/**
 * @brief Generate a key of random 'size' chars
 *
 * @param size
 *
 * @return generated string
 */
char *generate_key(size_t size) {
    struct timespec spec;
    char *key = malloc(size);

    if (!key)
        return NULL; // TODO: handle malloc error

    syscall(228, CLOCK_REALTIME, &spec); // clock_gettime syscall
    ft_srand(spec.tv_nsec);

    for (size_t i = 0; i < size; i++) {
        key[i] = ft_rand() % 256;
    }

    return key;
}

/**
 * @brief Encrypt the input with the generated key. Each char is XORed with the corresponding char in the key
 *
 * @param input input to encrypt
 * @param input_len length of input
 * @param key generated key
 *
 * @return encrypted string
 */
char *xor_encrypt(char *input, size_t input_len, t_key *key) {
    char *encrypted = malloc(input_len);

    if (!encrypted)
        return NULL;

    for (size_t i = 0, j = 0; i < input_len; i++, j++) {
        if (j == key->size)
            j = 0;
        encrypted[i] = input[i] ^ key->str[j];
    }

    return encrypted;
}
