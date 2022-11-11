#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <errno.h>
#include "s21_grep.h"


static int
regex_compile(t_strlist *template, regex_t **regex_array, int flags);
static int
grep(t_strlist *filename, regex_t *re, int nre, int flags);
static int
grep_from_file(FILE *stream, char *filename, regex_t *re, int nre, int flags);
static void
print_line(const char *line, size_t n, regex_t *re,
           int nre, const char *filename, int flags);

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
	// возможно добавить REG_NEWLINE, тогда переписывать '\n' на  '\0'
	// в считанной строке не обязательно

	n_template = strlist_size(template);
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
				name = "(standart input)";
			}
			else {
				name = filename->content;
			}
			status += grep_from_file(stream, name, re_array, nre, flags);
			if (ferror(stream) && (flags & SFLAG)) {
				print_error(name);
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

static int
grep_from_file(FILE *stream, char *filename, regex_t *re_array, int nre, int flags)
{
	char *nlptr = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	size_t line_counter = 0;
	size_t match_counter = 0;
	int file_matched = 0;
	int matched = REG_NOMATCH;
	int status = 0;

	while (!file_matched && (nread = getline(&line, &len, stream)) != -1) {
		errno = 0;
		matched = REG_NOMATCH;
		nlptr = strchr(line, '\n');
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
			if (flags & LFLAG) {
				puts(filename);
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
	if ((status == 0) && ((flags & LFLAG) == 0) && (flags & CFLAG)) {
		if (flags & PRINT_FILENAME) {
			printf("%s:", filename);
		}
		printf("%zu\n", match_counter);
	}
	free(line);

	return (status);
}

static void
print_line(const char *line, size_t n, regex_t *re,
           int nre, const char *filename, int flags)
{
	size_t nmatch = 1;
	regmatch_t pmatch[nmatch];
	int len = 0;
	regoff_t so = -1;
	regoff_t eo = -1;

#ifdef __linux__
	if (flags & OFLAG) {
#else /* MACOS */
	if (flags & OFLAG && (flags & VFLAG) == 0) {
#endif
		while (*line) {
			so = -1;
			for (int i = 0; i < nre; ++i) {
				int tmp = regexec(re + i, line, 1, pmatch, 0);
				if (tmp == 0) {
					if (pmatch->rm_so != -1 &&
						((so == -1) || pmatch->rm_so < so)) {
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
