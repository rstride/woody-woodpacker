#include "woody.h"

int write_file(const char *filename, const char *content, long size) {
    int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (fd < 0) return errno;

    long written = 0;
    while (written < size) {
        long chunk_size = (size - written >= 4096) ? 4096 : size - written;
        long result = write(fd, content + written, chunk_size);
        if (result < 0) {
            close(fd);
            return errno;
        }
        written += result;
    }
    close(fd);
    return 0;
}

int create_woody_file(void *addr, long size) {
    t_elf elf;
    t_key key;
    int ret = init_elf(&elf, addr, size);
    if (ret) return ret;

    key.size = KEY_SIZE;
    key.str = generate_key(key.size);

    int type = 0;
    Elf64_Phdr *next = elf.pt_load + 1;
    if (elf.pt_load->p_offset + elf.pt_load->p_memsz + INJECT_SIZE + key.size > next->p_offset) {
        type = ADD_PADDING;
        Elf64_Phdr *nnext = next + 1;
        if (nnext && nnext->p_type == PT_LOAD) return OUT_OF_RANGE;
    }

    long size_dst = size;
    if (type == ADD_PADDING) {
        size_dst += (((INJECT_SIZE + key.size) / PAGE_SIZE) + 1) * PAGE_SIZE;
    }

    void *dst = malloc(size_dst);
    if (!dst) {
        free(key.str);
        return MALLOC_ERROR;
    }

    ret = fill_binary(&elf, &key, dst, type);
    if (ret) {
        free(dst);
        free(key.str);
        return OUTPUT_ERROR;
    }

    ret = write_file("woody", dst, size_dst);
    free(dst);
    if (ret) {
        free(key.str);
        return OUTPUT_ERROR;
    }

    write(STDOUT_FILENO, "========================== KEY =========================\n", 57);
    ft_print_memory(key.str, key.size);
    write(STDOUT_FILENO, "=================== COPY/PASTE FORMAT ==================\n", 57);
    print_hexa_key(key.str, key.size);
    free(key.str);

    return 0;
}

int woody_woodpacker(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return errno;

    if (read(fd, NULL, 0) < 0) {
        close(fd);
        return errno;
    }

    long size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        close(fd);
        return errno ? errno : EINVAL;
    }

    void *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        close(fd);
        return errno;
    }

    int ret = check_file(addr);
    if (ret == ET_EXEC || ret == ET_DYN) {
        ret = create_woody_file(addr, size);
    } else {
        ret = WRONG_FILETYPE;
    }

    munmap(addr, size);
    close(fd);
    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return EINVAL;
    }

    int ret = woody_woodpacker(argv[1]);
    if (ret) print_error(argv, ret);

    return ret;
}
