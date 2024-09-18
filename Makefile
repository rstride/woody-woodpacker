SHELL := /bin/bash

# COLORS #
RED    := \e[31m
GREEN  := \e[32m
YELLOW := \e[33m
BLUE   := \e[34m
END    := \e[0m

# COMPILATION #
CC_FLAGS   := -Wall -Wextra -Werror
NASM_FLAGS := -f elf64

# DIRECTORIES #
DIR_HEADERS   := ./inc/
DIR_SRCS      := ./src/
DIR_ASM       := ./asm/
DIR_OBJS      := ./compiled_srcs/
DIR_OBJS_ASM  := ./compiled_srcs/

# FILES #
SRCS      := main.c binary.c elf.c encrypt.c padding.c print.c utils.c
SRCS_ASM  := inject.s

# COMPILED_SOURCES #
OBJS      := $(addprefix $(DIR_OBJS), $(SRCS:.c=.o))
OBJS_ASM  := $(addprefix $(DIR_OBJS_ASM), $(SRCS_ASM:.s=.o))
NAME      := woody_woodpacker

# CMDS #
INJECT    := readelf -x .text $(DIR_OBJS)$(basename $(SRCS_ASM)) | awk '{if(NR>2)print}' | sed -e '$$d' | sed 's/  //' | cut -f 2- -d ' ' | cut -d ' ' -f 1,2,3,4 | sed 's/ //g' | sed 's/\n//g' | tr -d '\n' | sed 's/../\\\\x&/g'
SIZE_INJECT := (echo -n "("; (wc -c <<< $(shell $(INJECT))) | xargs echo -n; echo " - 1) / 4") | bc

## RULES ##
all: $(NAME)

debug: CC_FLAGS += -g3 -fsanitize=address
debug: all

# VARIABLES RULES #
$(NAME): $(OBJS_ASM) $(OBJS)
	@printf "\033[2K\r$(BLUE) All files compiled into '$(DIR_OBJS)'. $(END)✅\n"
	@gcc $(CC_FLAGS) -I $(DIR_HEADERS) $(OBJS) -o $(NAME)
	@printf "\033[2K\r$(GREEN) Executable '$(NAME)' created. $(END)✅\n"

# COMPILED_SOURCES RULES #
$(OBJS): | $(DIR_OBJS)
$(OBJS_ASM): | $(DIR_OBJS_ASM)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.c
	@printf "\033[2K\r $(YELLOW)Compiling $< $(END)⌛ "
	@gcc $(CC_FLAGS) -D INJECT=\"$(shell $(INJECT))\" -D INJECT_SIZE=$(shell $(SIZE_INJECT)) -I $(DIR_HEADERS) -c $< -o $@

$(DIR_OBJS_ASM)%.o: $(DIR_ASM)%.s
	@printf "\033[2K\r $(YELLOW)Compiling $< $(END)⌛ "
	@nasm $(NASM_FLAGS) -o $@ $<
	@ld $@ -o $(basename $@)

$(DIR_OBJS):
	@mkdir -p $(DIR_OBJS)

# MANDATORY PART #
clean:
	@rm -rf $(DIR_OBJS) woody
	@printf "\033[2K\r$(RED) '$(DIR_OBJS)' and woody have been deleted. $(END)🗑️\n"

fclean: clean
	@rm -rf $(NAME)
	@printf "\033[2K\r$(RED) '$(NAME)' has been deleted. $(END)🗑️\n"

re: fclean all

# PHONY #
.PHONY: all debug clean fclean re
