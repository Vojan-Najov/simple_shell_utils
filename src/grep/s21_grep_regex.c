#include <stdlib.h>
#include "s21_grep.h"
#include <stdio.h>

int regex_compile(t_strlist *template, regex_t **regex_array, int flags)
{
	int n_template = 0;
	int i = 0;
	regex_t *re_array = NULL;
	int errcode = 0;
	char errbuf[MAX_ERR_LENGTH];
	int cflags = 0;
	

	if (flags & IFLAG)
		cflags |= REG_ICASE;
	if ((flags & OFLAG) == 0)
		cflags |= REG_NOSUB;
	// возможно добавить REG_NEWLINE, тогда переписывать '\n' на  '\0'
	// в считанной строке не обязательно

	n_template = strlist_size(template);
	re_array = (regex_t *) malloc(sizeof(regex_t) * n_template);
	if (re_array) {
		while ((errcode == 0) && (template != NULL)) {
			errcode = regcomp(re_array + i, template->content, cflags);
			template = template->next;
			++i;
		}
		if (errcode) {
			regerror(errcode, re_array + i, errbuf, MAX_ERR_LENGTH);
			fprintf(stderr, "s21_grep: %s\n", errbuf);
			while (i >= 0) {
				regfree(re_array + i);
				--i;
			}
			free(re_array);
			re_array = NULL;
		}
	}

	*regex_array = re_array;

	return (errcode == 0 ? n_template : -1);
}
