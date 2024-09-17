#include "woody.h"
#include <sys/stat.h>

/* Write content to the specified file */
int write_file(char *filename, char *content, long size) {
    int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0755);
    if (fd < 0)
        return errno;

    ssize_t total_written = 0;
    while (total_written < size) {
        ssize_t bytes_written = write(fd, content + total_written, size - total_written);
        if (bytes_written < 0) {
            close(fd);
            return errno;
        }
        total_written += bytes_written;
    }
    close(fd);
    return 0;
}

/* Generate key, allocate memory, and create the woody file */
int create_woody_file(void *addr, size_t size) {
    t_elf elf;
    int ret = init_elf(&elf, addr, size);
    if (ret != 0) {
        return ret;
    }

    t_key key;
    key.size = KEY_SIZE;
    key.str = generate_key(key.size);
    if (!key.str) {
        return MALLOC_ERROR;
    }

    int type = 0;
    Elf64_Phdr *next = elf.pt_load + 1;
    if (elf.pt_load->p_offset + elf.pt_load->p_memsz + INJECT_SIZE + key.size > next->p_offset) {
        type = ADD_PADDING;
        Elf64_Phdr *nnext = next + 1;
        if (nnext && nnext->p_type == PT_LOAD) {
            free(key.str);
            return OUT_OF_RANGE;
        }
    }

    size_t size_dst = size;
    if (type == ADD_PADDING) {
        size_t extra_size = ((INJECT_SIZE + key.size) / PAGE_SIZE + 1) * PAGE_SIZE;
        size_dst += extra_size;
    }

    void *dst = malloc(size_dst);
    if (!dst) {
        free(key.str);
        return MALLOC_ERROR;
    }

    ret = fill_binary(&elf, &key, dst, type);
    if (ret != 0) {
        free(dst);
        free(key.str);
        return ret;
    }

    ret = write_file("woody", dst, size_dst);
    free(dst);
    if (ret != 0) {
        free(key.str);
        return OUTPUT_ERROR;
    }

    printf("========================== KEY =========================\n");
    ft_print_memory(key.str, key.size);
    printf("=================== COPY/PASTE FORMAT ==================\n");
    print_hexa_key(key.str, key.size);
    printf("\n");

    free(key.str);
    return 0;
}


/* Open and map the file, then create the woody file */
int woody_woodpacker(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return errno;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return errno;
    }

    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "%s is not a regular file.\n", filename);
        close(fd);
        return EINVAL;
    }

    size_t size = st.st_size;
    void *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return errno;
    }
    close(fd);

    int file_type = check_file(addr);
    if (file_type != ET_EXEC && file_type != ET_DYN) {
        fprintf(stderr, "%s: Invalid or unsupported file type\n", filename);
        munmap(addr, size);
        return WRONG_FILETYPE;
    }

    int ret = create_woody_file(addr, size);
    munmap(addr, size);
    return ret;
}


/* Main function: check arguments, call woody_woodpacker, and handle errors */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <target_binary>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int ret = woody_woodpacker(argv[1]);
    if (ret != 0) {
        print_error(argv, ret);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

