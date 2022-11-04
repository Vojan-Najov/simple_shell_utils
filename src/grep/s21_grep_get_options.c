#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "s21_grep.h"
#include "get_next_line.h"

static int
get_options_error(t_string_list *template, t_string_list *filename);
static int
get_template_from_file(t_string_list **template, const char *filename);

int get_options(int argc, char *argv[])
{
	const char *opt_string = "e:ivclnhsf:o";
	int flags = 0;
	int c = -1;
	t_string_list *template = NULL;
	t_string_list *filename = NULL;

	while ((c = getopt(argc, argv, opt_string)) != -1) {
		switch (c) {
			case 'e':
				flags |= EFLAG;
				if (string_list_push_back(&template, optarg, 1) != 0) {
					return (get_options_error(template, filename));
				}
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
				get_template_from_file(&template, optarg);
				break;
			case 'o':
				flags |= OFLAG;
				break;
			case '?':
				fprintf(stderr, "Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n");
				return (get_options_error(template, filename));
		}
	}

	while (template) {
		printf("%s\n", template->content);
		template = template->next;
	}

	return flags;
}

static int
get_options_error(t_string_list *template, t_string_list *filename) {
	string_list_clear(template);
	string_list_clear(filename);

	return ERROR;
}

static int
get_template_from_file(t_string_list **template, const char *filename) {
	int fd = -1;
	int ret = -1;
	char *line = NULL;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		return (print_error(filename));
	}
	while ((ret = get_next_line(fd, &line)) > 0) {
		if (string_list_push_back(template, line, 0) != 0) {
			close(fd);
			//get_next_line_free_buffer;
			return (ERROR);
		}
	}
	if (ret == -1) {
		return (ERROR);
	}

	return (0);
}
