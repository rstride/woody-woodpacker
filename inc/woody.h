#ifndef WOODY_H
#define WOODY_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <string.h>
#include <zlib.h>
#include <time.h>

#define KEY_LEN 16

void handle_32bit_binary(void *file_content, size_t file_size, char *custom_key);
void handle_64bit_binary(void *file_content, size_t file_size, char *custom_key);
void encrypt_binary(char *data, size_t size, char *key, size_t key_len);
void decrypt_binary(char *data, size_t size, char *key, size_t key_len);
void generate_random_key(char *key, size_t key_len);
void compress_binary(unsigned char *input, size_t input_size, unsigned char *output, size_t *output_size);
void decompress_binary(unsigned char *input, size_t input_size, unsigned char *output, size_t *output_size);
extern void encrypt_binary_asm(char *data, char *key, size_t size);
extern void decrypt_binary_asm(char *data, char *key, size_t size);

#endif
