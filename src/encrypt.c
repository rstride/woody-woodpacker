#include "woody.h"
 // For compression and decompression

#define KEY_LEN 16

void handle_64bit_binary(void *file_content, size_t file_size, char *custom_key) {
    char key[KEY_LEN];
    
    // If no custom key, generate a random key
    if (custom_key == NULL) {
        generate_random_key(key, KEY_LEN);
        printf("Generated random key: %s\n", key);
    } else {
        strncpy(key, custom_key, KEY_LEN);
        printf("Using custom key: %s\n", key);
    }

    // Compress the binary before encryption
    unsigned char *compressed_data = malloc(file_size * 2); // Allocate extra space
    size_t compressed_size = file_size * 2;
    compress_binary(file_content, file_size, compressed_data, &compressed_size);

    // Encrypt the binary
    encrypt_binary((char *)compressed_data, compressed_size, key, KEY_LEN);

    // Write encrypted binary to a new file
    FILE *output = fopen("woody", "wb");
    fwrite(compressed_data, 1, compressed_size, output);
    fclose(output);

    free(compressed_data);
}

void handle_32bit_binary(void *file_content, size_t file_size, char *custom_key) {
    // Similar to handle_64bit_binary but for 32-bit
    // Add the logic to handle ELFCLASS32 here
    char key[KEY_LEN];
    
    // If no custom key, generate a random key
    if (custom_key == NULL) {
        generate_random_key(key, KEY_LEN);
        printf("Generated random key: %s\n", key);
    } else {
        strncpy(key, custom_key, KEY_LEN);
        printf("Using custom key: %s\n", key);
    }

    // Compress the binary before encryption
    unsigned char *compressed_data = malloc(file_size * 2); // Allocate extra space
    size_t compressed_size = file_size * 2;
    compress_binary(file_content, file_size, compressed_data, &compressed_size);

    // Encrypt the binary
    encrypt_binary((char *)compressed_data, compressed_size, key, KEY_LEN);

    // Write encrypted binary to a new file
    FILE *output = fopen("woody", "wb");
    fwrite(compressed_data, 1, compressed_size, output);
    fclose(output);

    free(compressed_data);
}

// XOR encryption for now (you can replace this with AES or other complex algorithms)
void encrypt_binary(char *data, size_t size, char *key, size_t key_len) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % key_len];
    }
}

void decrypt_binary(char *data, size_t size, char *key, size_t key_len) {
    // Simple XOR decryption (same logic as encryption)
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % key_len];
    }
}

void generate_random_key(char *key, size_t key_len) {
    srand(time(NULL));
    for (size_t i = 0; i < key_len; i++) {
        key[i] = rand() % 256;
    }
}

// zlib compression
void compress_binary(unsigned char *input, size_t input_size, unsigned char *output, size_t *output_size) {
    compress(output, output_size, input, input_size);
}

void decompress_binary(unsigned char *input, size_t input_size, unsigned char *output, size_t *output_size) {
    uncompress(output, output_size, input, input_size);
}
