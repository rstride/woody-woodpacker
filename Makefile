NAME = ft_malcolm

CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./includes

SRC = src/main.c \
	  src/malcolm.c \
	  src/utils.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all