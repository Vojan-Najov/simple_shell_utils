NAME = s21_cat

INCLUDES = s21_cat.h

SRC = s21_cat.c s21_cat_utils.c

OBJ = $(SRC:.c=.o)

CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11 -Wpedantic

$(NAME): $(OBJ)
	$(CC) $^ -o $@

%.o: %.c $(INCLUDES)
	gcc $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
