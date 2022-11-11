NAME = s21_grep

SRC = s21_grep.c s21_grep_get_options.c s21_grep_utils.c

OBJ = $(SRC:.c=.o)

CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11 -I ./

$(NAME): $(OBJ)
	$(CC) $^ -o $@

.c.o:
	gcc $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
