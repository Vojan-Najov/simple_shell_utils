#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "s21_grep.h"

int strlist_push_back(t_strlist **head, char *content, int need_free)
{
	int status = 0;

	while (*head != NULL) {
		head = &(*head)->next;
	}

	*head = (t_strlist *) malloc(sizeof(t_strlist));
	if (*head != NULL) {
		(*head)->content = content;
		(*head)->need_free = need_free;
		(*head)->next = NULL;
	} else {
		print_error("memory error");
		status = ERROR;
	}

	return (status);
}

int strlist_insert_sort(t_strlist **head, char *content, int need_free)
{
	int status = 0;
	t_strlist *tmp = NULL;

	while ((*head != NULL) && (strcmp((*head)->content, content) < 0)) {
		head = &(*head)->next;
	}

	if (*head != NULL) {
		tmp = *head;
	}
	*head = (t_strlist *) malloc(sizeof(t_strlist));
	if (*head != NULL) {
		(*head)->content = content;
		(*head)->need_free = need_free;
		(*head)->next = tmp;
	}
	else {
		print_error("memory error");
		status = ERROR;
	}

	return (status);
}

size_t strlist_size(t_strlist *head)
{
	size_t size = 0;

	while (head) {
		++size;
		head = head->next;
	}

	return (size);
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

	if (tag[0] == '-' && tag[1] == '\0') {
		tag = "standart input";
	}

	str_error = strerror(errnum);
	fflush(stdout);
	fprintf(stderr, "s21_grep: %s: %s\n", tag, str_error);
	
	return errnum;
}

FILE *open_file(const char *filename, int no_message) {
	FILE *stream;

	if (filename[0] == '-' && filename[1] == '\0') {
		stream = stdin;
	}
	else {
		stream = fopen(filename, "r");
	}

	if (stream == NULL && !no_message) {
		print_error(filename);
	}

	return (stream);
}
