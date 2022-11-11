NAME = s21_cat

SRC = s21_cat.c s21_cat_utils.c

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
