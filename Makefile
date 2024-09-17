CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./inc
SRC = src/main.c src/encrypt.c
OBJ = $(SRC:.c=.o)
NAME = woody_woodpacker

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lz 

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
