#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "s21_grep.h"


int grep(t_strlist *filename, regex_t *re, int nre, int flags);
void print_line(const char *line, size_t n, regex_t *re, int nre, const char *filename, int flags);
void grep_from_file(FILE *stream, char *filename, regex_t *re, int nre, int flags);

int main(int argc, char *argv[])
{
	int flags = 0;
	int status = 0;
	int n_template = 0;
	regex_t *re_array = NULL;
	t_strlist *template = NULL;
	t_strlist *filename = NULL;

	flags = get_options(argc, argv, &template, &filename);
	if (flags != ERROR) {
		n_template = regex_compile(template, &re_array, flags);
		if (n_template > 0) {
			status = grep(filename, re_array, n_template, flags);
			for (int i = 0; i < n_template; ++i) {
				regfree(re_array + i);
			}
			free(re_array);
		}
		else {
			status = 1;
		}
	}
	else {
		status = 1;
	}

	strlist_clear(template);
	strlist_clear(filename);

	return (status);
}

int grep(t_strlist *filename, regex_t *re_array, int nre, int flags)
{
	FILE *stream = NULL;
	char *name = NULL;
	int status = 0;

	if (((flags & HFLAG) == 0) && (strlist_size(filename) > 1)) {
		flags |= PRINT_FILENAME;
	}

	while (filename) {
		stream = open_file(filename->content, flags & SFLAG);
		if (stream != NULL) {
			if (strcmp(filename->content, "-") == 0) {
				name = "(standart input)";
			}
			else {
				name = filename->content;
			}
			//status += grep_from_file(stream, name, flags);
			grep_from_file(stream, name, re_array, nre, flags);
			if (stream == stdin) {
				clearerr(stdin);
			}
			else {
				fclose(stream);
			}
		}
		filename = filename->next;
	}

	return status;
}

void grep_from_file(FILE *stream, char *filename, regex_t *re_array, int nre,
                    int flags)
{
	char *nlptr = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	size_t line_counter = 0;
	size_t match_counter = 0;
	int file_matched = 0;
	int matched = REG_NOMATCH;

	while (!file_matched && (nread = getline(&line, &len, stream)) != -1) {
		matched = REG_NOMATCH;
		nlptr = index(line, '\n');
		if (nlptr != NULL) {
			*nlptr = '\0';
		}
		++line_counter;

		for (int i = 0; (matched == REG_NOMATCH) && (i < nre); ++i) {
			matched = regexec(re_array + i, line, 0, NULL, 0);
		}
		matched = matched == 0 ? 1 : 0;
		if (flags & VFLAG) {
			matched = (matched + 1) % 2;
		}
		if (matched) {
			++match_counter;
			//printf("matched = %zu\n", match_counter);
			if (flags & LFLAG) {
				puts(filename);
				file_matched = 1;
			}
			else if ((flags & CFLAG) == 0) {
				print_line(line, line_counter, re_array, nre, filename, flags);
			}
		}
	}
	if (((flags & LFLAG) == 0) && (flags & CFLAG)) {
		if (flags & PRINT_FILENAME) {
			printf("%s:", filename);
		}
		printf("%zu\n", match_counter);
	}
}

void print_line(const char *line, size_t n, regex_t *re, int nre,
                const char *filename, int flags)
{
	regmatch_t *pmatch = (regmatch_t *) malloc(sizeof(regmatch_t) * nre);
	int matched = REG_NOMATCH;
	int len = 0;

	//if (flags & OFLAG && (flags & VFLAG) == 0) { // MACOS
	if (flags & OFLAG) { // LINUX
		while (*line) {
			int min_so = 1000000;
			int min_eo = -1;
			for (int i = 0; i < nre; ++i) {
				int tmp = regexec(re + i, line, 1, pmatch + i, 0);
				if (tmp == 0 && (pmatch[i].rm_so < min_so)) {
					min_so = pmatch[i].rm_so;
					min_eo = pmatch[i].rm_eo;
				
					++matched;
				}
			}
			if (matched && min_so != 1000000) {
					line += min_so; 
					len = min_eo - min_so;
					if (flags & PRINT_FILENAME) {
						printf("%s:", filename);
					}
					if (flags & NFLAG) {
						printf("%zu:", n);
					}
					printf("%.*s\n", len, line);
					line += len;
				}
			else {
				line = "";
			}
		}
	}
	else {
		if (flags & PRINT_FILENAME) {
			printf("%s:", filename);
		}
		if (flags & NFLAG) {
			printf("%zu:", n);
		}
		printf("%s\n", line);
	}
}

/*
void print_line(const char *line, size_t n, regex_t *re, int nre,
                const char *filename, int flags)
{
	size_t nmatch = 1;
	regmatch_t pmatch[nmatch];
	int matched = REG_NOMATCH;
	int len = 0;

	if (flags & OFLAG) {
		while (*line) {
			matched = REG_NOMATCH;
			for (int i = 0; (matched == REG_NOMATCH) && (i < nre); ++i) {
				matched = regexec(re + i, line, nmatch, pmatch, 0);
				//printf("matched = %d\n", matched);
			}
			if (matched == 0) {
				//	printf("so = %d  eo = %d\n", pmatch[0].rm_so, pmatch[0].rm_eo);
					line += pmatch[0].rm_so; 
					len = pmatch[0].rm_eo - pmatch[0].rm_so;
					if (flags & PRINT_FILENAME) {
						printf("%s:", filename);
					}
					if (flags & NFLAG) {
						printf("%zu:", n);
					}
					printf("%.*s\n", len, line);
					line += len;
				//	printf("line is _%s_\n", line);
				}
			else {
				line = "";
			}
		}
	}
	else {
		if (flags & PRINT_FILENAME) {
			printf("%s:", filename);
		}
		if (flags & NFLAG) {
			printf("%zu:", n);
		}
		printf("%s\n", line);
	}
}
*/
