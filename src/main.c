#include "woody.h"

void print_usage(char *prog_name) {
    printf("Usage: %s [-k key] file\n", prog_name);
}

int main(int argc, char *argv[]) {
    int opt;
    char *custom_key = NULL;
    char *input_file = NULL;

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, "k:")) != -1) {
        switch (opt) {
        case 'k':
            custom_key = optarg;
            break;
        default:
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Get the input file
    if (optind >= argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_file = argv[optind];

    // Open the input file
    int fd = open(input_file, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("Error getting file size");
        close(fd);
        return EXIT_FAILURE;
    }

    // Memory-map the file
    void *file_content = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return EXIT_FAILURE;
    }

    // Check if the file is a valid ELF binary
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *)file_content;
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not a valid ELF file.\n");
        munmap(file_content, st.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    // Determine if the file is 32-bit or 64-bit
    if (elf_header->e_ident[EI_CLASS] == ELFCLASS32) {
        printf("Detected 32-bit ELF binary.\n");
        handle_32bit_binary(file_content, st.st_size, custom_key);
    } else if (elf_header->e_ident[EI_CLASS] == ELFCLASS64) {
        printf("Detected 64-bit ELF binary.\n");
        handle_64bit_binary(file_content, st.st_size, custom_key);
    } else {
        fprintf(stderr, "Unsupported ELF class\n");
        munmap(file_content, st.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    // Clean up
    munmap(file_content, st.st_size);
    close(fd);
    return EXIT_SUCCESS;
}
