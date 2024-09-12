#include "woody.h"

void generate_random_key(unsigned char *key, size_t size) {
    srand(time(NULL));
    for (size_t i = 0; i < size; i++) {
        key[i] = rand() % 256;
    }
}

void xor_encrypt(unsigned char *data, size_t size, unsigned char *key, size_t key_size) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % key_size];
    }
}

int woody(char *file) {
    void *data = readable(file);
    if (data == NULL) {
        return 1;
    }

    struct stat st;
    if (stat(file, &st) < 0) {
        perror("stat: failed to get file size");
        return 1;
    }

    size_t file_size = st.st_size;

    // Generate a random key
    unsigned char key[16];  // 128-bit key
    generate_random_key(key, sizeof(key));

    // Display the key
    printf("key_value: ");
    for (size_t i = 0; i < sizeof(key); i++) {
        printf("%02X", key[i]);
    }
    printf("\n");

    // Encrypt the file using XOR
    xor_encrypt((unsigned char *)data, file_size, key, sizeof(key));

    // Write the encrypted file to "woody"
    int fd = open("woody", O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd < 0) {
        perror("open: failed to create 'woody'");
        return 1;
    }

    if (write(fd, data, file_size) != file_size) {
        perror("write: failed to write to 'woody'");
        close(fd);
        return 1;
    }

    close(fd);

    // Insert the string "....WOODY...." when the program is executed
    printf("....WOODY....\n");

    return 0;
}