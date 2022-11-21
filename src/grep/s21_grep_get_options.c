#define _POSIX_C_SOURCE 200809L
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "s21_grep.h"

static int
get_template_from_file(t_strlist **template, const char *filename);
static int
get_template_from_args(t_strlist **template, char *arg);
static int
get_filename_from_args(t_strlist **filename, char **arg_ptr);

int
get_options(int argc, char **argv, t_strlist **template, t_strlist **filename)
{
	const char *optstring = "e:ivclnhsf:o";
	int flags = 0;
	int c = -1;
	int status = 0;
	char **arg_ptr = NULL;

	opterr = 0;
	while ((status == 0) && \
	((c = getopt_long(argc, argv, optstring, NULL, NULL)) != -1)) {
		switch (c) {
			case 'e':
				flags |= EFLAG;
				status = strlist_insert_sort(template, optarg, NO_NEED_TO_FREE);
				break;
			case 'i':
				flags |= IFLAG;
				break;
			case 'v':
				flags |= VFLAG;
				break;
			case 'c':
				flags |= CFLAG;
				break;
			case 'l':
				flags |= LFLAG;
				break;
			case 'n':
				flags |= NFLAG;
				break;
			case 'h':
				flags |= HFLAG;
				break;
			case 's':
				flags |= SFLAG;
				break;
			case 'f':
				flags |= FFLAG;
				status = get_template_from_file(template, optarg);
				break;
			case 'o':
				flags |= OFLAG;
				break;
			case '?':
				fprintf(stderr, "s21_grep: invalid option -- '%c'\n", optopt);
				print_usage();
				status = ERROR;
				break;
		}
	}

	if ((flags & FFLAG) && *template == NULL) {
		status = ERROR;
	}

	if (status == 0) {
		arg_ptr = argv + optind;
		if (*template == NULL) {
			status = get_template_from_args(template, *arg_ptr);
			++arg_ptr;
		}
		if (status == 0) {
			status = get_filename_from_args(filename, arg_ptr);
		}
	}
	
	return (status == 0 ? flags : ERROR);
}

static int get_template_from_args(t_strlist **template, char *arg)
{
	int status = 0;

	if (arg != NULL) {
		status = strlist_push_back(template, arg, NO_NEED_TO_FREE);
	}
	else {
		print_usage();
		status = ERROR;
	}

	return (status);
}

static int get_filename_from_args(t_strlist **filename, char **arg_ptr)
{
	int status = 0;

	if (*arg_ptr == NULL) {
		status = strlist_push_back(filename, "-", NO_NEED_TO_FREE);
	}
	while ((status == 0) && (*arg_ptr != NULL)) {
		status = strlist_push_back(filename, *arg_ptr, NO_NEED_TO_FREE);
		++arg_ptr;
	}

	return status;
}

static int
get_template_from_file(t_strlist **template, const char *filename)
{
	FILE *stream;
	ssize_t nread;
	int status = 0;
	char *line = NULL;
	size_t len = 0;

	stream = open_file(filename, 0);
	if (stream == NULL) {
		status = ERROR;
	}
	else {
		errno = 0;
		while ((status == 0) && ((nread = getline(&line, &len, stream)) != -1)) {
			if (line[nread - 1] == '\n')
				line[nread - 1] = '\0';
			status = strlist_insert_sort(template, line, NEED_TO_FREE);
			line = NULL;
			len = 0;
			errno = 0;
		}
		free(line);

		if (ferror(stream)) {
			print_error(filename);
			status = ERROR;
		}
		else if (nread == -1 && errno == ENOMEM) {
			print_error("memory error");
			status = ERROR;
		}

		if (stream == stdin) {
			clearerr(stdin);
		}
		else {
			fclose(stream);
		}
	}

	return (status);
}
