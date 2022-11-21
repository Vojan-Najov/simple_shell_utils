#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <errno.h>
#include "s21_grep.h"


static int regex_compile(t_strlist *template, regex_t **regex_array, int flags);
static int grep(t_strlist *filename, regex_t *re, int nre, int flags);
static int grep_from_file(FILE *stream, const char *filename,
                          regex_t *re, int nre, int flags);
static void print_line(const char *line, size_t n, regex_t *re,
                       int nre, const char *filename, int flags);

static int grep_matched = 0;

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

	return (status ? 2 : grep_matched ? 0 : 1);
}

static int
regex_compile(t_strlist *template, regex_t **regex_array, int flags)
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

	n_template = (int) strlist_size(template);
	re_array = (regex_t *) calloc(n_template, sizeof(regex_t));
	if (re_array) {
		while ((errcode == 0) && (template != NULL)) {
			errcode = regcomp(re_array + i, template->content, cflags);
			template = template->next;
			++i;
		}
		if (errcode) {
			regerror(errcode, re_array + i, errbuf, MAX_ERR_LENGTH);
			fprintf(stderr, "s21_grep: %s\n", errbuf);
			while (--i >= 0) {
				regfree(re_array + i);
			}
			free(re_array);
			re_array = NULL;
		}
	}

	*regex_array = re_array;

	return (errcode == 0 ? n_template : -1);
}

static int
grep(t_strlist *filename, regex_t *re_array, int nre, int flags)
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
				name = "(standard input)";
			}
			else {
				name = filename->content;
			}
			status += grep_from_file(stream, name, re_array, nre, flags);
			if (ferror(stream)) {
				if (!(flags & SFLAG)) {
					print_error(name);
				}
				++status;
			}
			if (ferror(stdout)) {
				print_error("(stdandard output)");
				++status;
			}
			if (stream == stdin) {
				clearerr(stdin);
			}
			else {
				fclose(stream);
			}
		}
		else {
			++status;
		}
		filename = filename->next;
	}

	return (status);
}

static int grep_from_file(FILE *stream, const char *filename,
                          regex_t *re_array, int nre, int flags)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	size_t line_counter = 0;
	size_t match_counter = 0;
	int file_matched = 0;
	int matched = REG_NOMATCH;
	int status = 0;

	while (!file_matched && ((nread = getline(&line, &len, stream)) != -1)) {
		errno = 0;
		matched = REG_NOMATCH;
		++line_counter;
		if (line[nread - 1] == '\n') {
			line[nread - 1] = '\0';
		}

		for (int i = 0; (matched == REG_NOMATCH) && (i < nre); ++i) {
			matched = regexec(re_array + i, line, 0, NULL, 0);
		}
		matched = matched == 0 ? 1 : 0;
		if (flags & VFLAG) {
			matched = (matched + 1) % 2;
		}
		if (matched) {
			++match_counter;
			if (flags & LFLAG) {
				file_matched = 1;
			}
			else if ((flags & CFLAG) == 0) {
				print_line(line, line_counter, re_array, nre, filename, flags);
			}
		}
	}
	if (nread == -1 && errno == ENOMEM) {
		print_error("memory error");
		status = 1;
	}
	free(line);

	if (flags & LFLAG) {
		if  (file_matched) {
			puts(filename);
		}
	}
	else if (flags & CFLAG) {
		if (flags & PRINT_FILENAME) {
			printf("%s:", filename);
		}
		printf("%zu\n", match_counter);
	}
	if (match_counter)
		grep_matched = 1;

	return (status);
}

static void print_line(const char *line, size_t n, regex_t *re,
                       int nre, const char *filename, int flags)
{
	size_t nmatch = 1;
	regmatch_t pmatch[nmatch];
	int len = 0;
	int eflags = 0;
	regoff_t so = -1;
	regoff_t eo = -1;

	if ((flags & OFLAG) && !(flags & VFLAG)) {
		while (*line) {
			so = -1;
			eo = -1;
			for (int i = 0; i < nre; ++i) {
				if (regexec(re + i, line, 1, pmatch, eflags) == 0) {
					if (so == -1 || pmatch->rm_so < so || 
					(pmatch->rm_so == so && \
					pmatch->rm_eo - pmatch->rm_so > eo - so)) {
						so = pmatch->rm_so;
						eo = pmatch->rm_eo;
					}	
				}
			}
			if (so != -1) {
				line += so; 
				len = (int) (eo - so);
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
			eflags = REG_NOTBOL;
		}
	}
	else if (!(flags & OFLAG)){
		if (flags & PRINT_FILENAME) {
			printf("%s:", filename);
		}
		if (flags & NFLAG) {
			printf("%zu:", n);
		}
		printf("%s\n", line);
	}
}
