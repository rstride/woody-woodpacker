#include "woody.h"

int verbose = PRINT_MODE;

int printHex(char *data, size_t size) {
	size_t i;

	for (i = 0; i < size; i++)
	{
		printf("%02x", data[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
		else if ((i + 1) % 4 == 0)
			printf(" ");
		else
			printf(" ");
	}
	printf("\n");
	return (0);
}

void *readable(char *file) {
	int fd;
	char *data;
	struct stat st;

	if ((fd = open(file, O_RDONLY)) < 0)
	{
		perror("open: failed to open file");
		return (NULL);
	}

	if (fstat(fd, &st) < 0)
	{
		perror("fstat: failed to fill stat structure");
		return (NULL);
	}

	if ((data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		perror("mmap: failed to map file");
		return (NULL);
	}

	if (verbose)
		printf("File %s is readable\n", file);

	close(fd);
	return (data);
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return (1);
    }

	void *data = readable(argv[1]);
	if (data == NULL)
		return (1);
	
	elf_header(data);

    // woody(argv[1]);
    return (0);
}