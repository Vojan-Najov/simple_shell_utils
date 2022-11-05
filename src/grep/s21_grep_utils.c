#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "s21_grep.h"

int strlist_push_back(t_strlist **head, char *content, int need_free)
{
	while (*head != NULL) {
		head = &(*head)->next;
	}
	*head = (t_strlist *) malloc(sizeof(t_strlist));
	if (*head == NULL) {
		return (print_error("memory error"));
	}
	(*head)->content = content;
	(*head)->need_free = need_free;
	(*head)->next = NULL;

	return (0);
}

void strlist_clear(t_strlist *head)
{
	t_strlist *tmp;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		if (tmp->need_free) {
			free(tmp->content);
		}
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
