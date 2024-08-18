#include "woody.h"

int check_magic(void *data) {
	if (data == NULL) {
		return 1;
	}

	Elf64_Ehdr *header = (Elf64_Ehdr *)data;
	if (header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 || header->e_ident[EI_MAG2] != ELFMAG2 || header->e_ident[EI_MAG3] != ELFMAG3) {
		return 1;
	}

	return 0;
}

void print_elf_header(Elf64_Ehdr *header) {
    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", header->e_ident[i]);
    }
    printf("\n");
    printf("  Class:                             %d: %d\n", header->e_ident[EI_CLASS], EI_CLASS);
    printf("  Data:                              %d\n", header->e_ident[EI_DATA]);
    printf("  Version:                           %d\n", header->e_ident[EI_VERSION]);
    printf("  OS/ABI:                            %d\n", header->e_ident[EI_OSABI]);
    printf("  ABI Version:                       %d\n", header->e_ident[EI_ABIVERSION]);
    printf("  Type:                              %d\n", header->e_type);
    printf("  Machine:                           %d\n", header->e_machine);
    printf("  Version:                           0x%x\n", header->e_version);
    printf("  Entry point address:               0x%lx\n", header->e_entry);
    printf("  Start of program headers:          %ld (bytes into file)\n", header->e_phoff);
    printf("  Start of section headers:          %ld (bytes into file)\n", header->e_shoff);
    printf("  Flags:                             0x%x\n", header->e_flags);
    printf("  Size of this header:               %d (bytes)\n", header->e_ehsize);
    printf("  Size of program headers:           %d (bytes)\n", header->e_phentsize);
    printf("  Number of program headers:         %d\n", header->e_phnum);
    printf("  Size of section headers:           %d (bytes)\n", header->e_shentsize);
    printf("  Number of section headers:         %d\n", header->e_shnum);
    printf("  Section header string table index: %d\n", header->e_shstrndx);
}

int elf_header(void *data) {
    if (check_magic(data) != 0) {
        printf("Invalid ELF file\n");
        return 1;
    }

    Elf64_Ehdr *header = (Elf64_Ehdr *)data;
    print_elf_header(header);

    return 0;
}