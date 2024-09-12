NAME = woody_woodpacker

CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./includes
LDFLAGS = -lz

SRC = srcs/main.c \
      srcs/elf.c \
      srcs/woody.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all