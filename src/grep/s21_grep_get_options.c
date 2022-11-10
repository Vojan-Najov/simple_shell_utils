#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include "s21_grep.h"

static int
get_template_from_file(t_strlist **template, const char *filename);
static int
get_template_from_args(t_strlist **template, char *arg);
static int
get_filename_from_args(t_strlist **filename, char **arg_ptr);

int
get_options(int argc, char *argv[], t_strlist **template, t_strlist **filename)
{
	const char *optstring = "e:ivclnhsf:o";
	int flags = 0;
	int c = -1;
	int status = 0;
	char **arg_ptr = NULL;

	while ((status == 0) && (c = getopt(argc, argv, optstring)) != -1) {
		switch (c) {
			case 'e':
				flags |= EFLAG;
				//status = strlist_push_back(template, optarg, 0);
				status = strlist_insert_sort(template, optarg, 0);
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
				fprintf(stderr, "Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n");
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
		status = strlist_push_back(template, arg, 0);
	}
	else {
		status = ERROR;
	}

	return (status);
}

static int get_filename_from_args(t_strlist **filename, char **arg_ptr)
{
	int status = 0;

	if (*arg_ptr == NULL) {
		status = strlist_push_back(filename, "-", 0);
	}
	while ((status == 0) && (*arg_ptr != NULL)) {
		status = strlist_push_back(filename, *arg_ptr, 0);
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
	char *nlptr;
	size_t len = 0;

	stream = open_file(filename, 0);
	if (stream == NULL) {
		status = ERROR;
	}
	else {
		errno = 0;
		while ((status == 0) && ((nread = getline(&line, &len, stream)) != -1)) {
			nlptr = index(line, '\n');
			if (nlptr) {
				*nlptr = '\0';
			}
			//status = strlist_push_back(template, line, 1);
			status = strlist_insert_sort(template, line, 1);
			if (status) {
				print_error("memory error");
			}
			line = NULL;
			len = 0;
			errno = 0;
		}

		if (ferror(stream)) {
			print_error(filename);
			status = ERROR;
		}
		else if (nread == -1 && errno) {
			print_error(filename);
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

/*
static int
get_template_from_file(t_strlist **template, const char *filename) {
	int fd = -1;
	int ret = -1;
	int status = 0;
	char *line = NULL;

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		print_error(filename);
		status = ERROR;
	}
	else {
		while ((ret = get_next_line(fd, &line)) > 0) {
			ret = strlist_push_back(template, line, 1);
			if (ret != 0) {
				status = ERROR;
				break;
			}
		}
		close(fd);
		if (ret == -1) {
			status = ERROR;
		}
	}

	return (status);
}
*/
