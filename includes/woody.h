#ifndef WOODY_H
#define WOODY_H

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <zlib.h>
#include <stdlib.h>
#include <time.h>
#include <elf.h>

void *readable(char *file);
int elf_header(void *data);
int check_magic(void *data);
int woody(char *file);

#define HELP_MESSAGE "Usage: %s <file>\n"

#define error int
#define NO_ERROR 0

#endif