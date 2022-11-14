#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "s21_cat.h"


int get_options(char **argv)
{
	int flags = 0;
	char *arg = NULL;
	char c = '\0';

	while ((arg = *++argv) && !(flags & HELP) && !(flags & ERROR))
	{
		if (arg[0] == '-' && arg[1] != '-' && arg[1] != '\0')
		{
			while ((c = *++arg))
			{
				switch (c)
				{
					case 'A':
						flags |= VFLAG | EFLAG | TFLAG;
						break;
					case 'b':
						flags |= BFLAG | NFLAG;
						break;
					case 'e':
						flags |= EFLAG | VFLAG;
						break;
					case 'E':
						flags |= EFLAG;
						break;
					case 'n':
						flags |= NFLAG;
						break;
					case 's':
						flags |= SFLAG;
						break;
					case 't':
						flags |= TFLAG | VFLAG;
						break;
					case 'T':
						flags |= TFLAG;
						break;
					case 'v':
						flags |= VFLAG;
						break;
					default:
						print_option_error(c);
						flags |= ERROR;
				}
			}
		}
		else if (arg[0] == '-' && arg[1] == '-')
		{
			arg += 2;
			if (strcmp(arg, "show-all") == 0) {
				flags |= VFLAG | EFLAG | TFLAG;
			}
			else if (strcmp(arg, "number-nonblank") == 0) {
				flags |= BFLAG | NFLAG;
			}
			else if (strcmp(arg, "show-ends") == 0) {
				flags |= EFLAG;
			}
			else if (strcmp(arg, "number") == 0) {
				flags |= NFLAG;
			}
			else if (strcmp(arg, "squeeze-blank") == 0) {
				flags |= SFLAG;
			}
			else if (strcmp(arg, "show-tabs") == 0) {
				flags |= TFLAG;
			}
			else if (strcmp(arg, "show-nonprinting") == 0) {
				flags |= VFLAG;
			}
			else if (strcmp(arg, "help") == 0) {
				flags |= HELP;
			}
			else {
				flags |= ERROR;
				print_long_option_error(arg - 2);
			}
		}
	}

	return flags;
}

int print_error(const char *tag)
{
	int errnum = errno;
	const char *str_error;

	if (tag[0] == '-' && tag[1] == '\0') {
		tag = "(standart input)";
	}

	str_error = strerror(errnum);
	fflush(stdout);
	fprintf(stderr, "s21_cat: %s: %s\n", tag, str_error);

	return errnum;
}

void print_option_error(int c)
{
	char message[] = "s21_cat: invalid option -- ";
	char buf[] = "'c'\n";
	size_t size = sizeof(char);

	buf[1] = c;
	fwrite(message, size, sizeof(message) - 1, stderr);
	fwrite(buf, size, sizeof(buf) - 1, stderr);
	fwrite(HELP_MESSAGE, size, sizeof(HELP_MESSAGE) - 1, stderr);
}

void print_long_option_error(const char *option)
{
	char message[] =  "s21_cat: unrecognized option '";
	char buf[] = "'\n";
	size_t size = sizeof(char);

	fwrite(message, size, sizeof(message) - 1, stderr);
	fwrite(option, size, strlen(option), stderr);
	fwrite(buf, size, sizeof(buf) - 1, stderr);
}

void print_usage(void)
{
	size_t size = sizeof(char);
	char usage[] =
      "Usage: cat [OPTION]... [FILE]...\n"
      "Concatenate FILE(s) to standard output.\n\n"
      "With no FILE, or when FILE is -, read standard input.\n\n"
      "  -A, --show-all           equivalent to -vET\n"
      "  -b, --number-nonblank    number nonempty output lines, overrides -n\n"
      "  -e                       equivalent to -vE\n"
      "  -E, --show-ends          display $ at end of each line\n"
      "  -n, --number             number all output lines\n"
      "  -s, --squeeze-blank      suppress repeated empty output lines\n"
      "  -t                       equivalent to -vT\n"
      "  -T, --show-tabs          display TAB characters as ^I\n"
      "  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n"
      "      --help               display this help and exit\n";

	fwrite(usage, size, sizeof(usage) - 1, stdout);
}
