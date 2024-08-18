NAME = woody_woodpacker

CC = gcc
CFLAGS = -Wall -Wextra -Werror -lz -I./includes

SRC = srcs/main.c \
	  srcs/elf.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all