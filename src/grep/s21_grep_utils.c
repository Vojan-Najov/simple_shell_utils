#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "s21_grep.h"

int string_list_push_back(t_string_list **head, char *content, int make_copy)
{
	char *copy = content;

	if (make_copy) {
		copy = strdup(content);
		if (copy == NULL) {
			return (print_error("memory error"));
		}
	}

	while (*head != NULL) {
		head = &(*head)->next;
	}
	*head = (t_string_list *) malloc(sizeof(t_string_list));
	if (*head == NULL) {
		return (print_error("memory error"));
	}
	(*head)->content = content;

	return (0);
}

void string_list_clear(t_string_list *head)
{
	t_string_list *tmp;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		free(tmp->content);
		free(tmp);
	}
}

int print_error(const char *tag)
{
	int errnum = errno;
	char *str_error;

	str_error = strerror(errnum);
	fflush(stdout);
	fprintf(stderr, "s21_grep: %s: %s\n", tag, str_error);
	
	return errnum;
}
