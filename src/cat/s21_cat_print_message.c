#include "s21_cat.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

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
      "  -u                       (ignored)\n"
      "  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n"
      "      --help               display this help and exit\n";

	fwrite(usage, size, sizeof(usage) - 1, stdout);
}

void print_file_error(const char *filename)
{
	char buf[] = "s21_cat: ";
	int errno_copy;

	errno_copy = errno;
	fwrite(buf , sizeof(char), sizeof(buf), stderr);
	errno = errno_copy;
	perror(filename);
}
