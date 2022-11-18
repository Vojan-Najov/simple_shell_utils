#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <s21_cat.h>


static int not_files_in_argv(char **argv);
static int cat(FILE *stream, const char *filename, int flags);
static int fastcat(FILE *stream, const char *filename);

int main(int argc, char **argv)
{
	int flags = 0;
	int fstdin = 0;
	FILE *stream = NULL;
	int status = 0;
	const char *filename;

	flags = get_options(argv);

	if (flags & ERROR) {
		status = 1;
	}
	else if (flags & HELP) {
		print_usage();
	}
	else {
		if (argc == 1 || not_files_in_argv(argv)) {
			argc = 2;
			fstdin = 1;
		}

		for (int i = 1; i < argc; ++i) {
			if (fstdin || (argv[i][0] == '-' && argv[i][1] == '\0')) {
				filename = "(standart input)";
				stream = stdin;
			}
			else if (argv[i][0] != '-') {
				filename = argv[i];
				stream = fopen(argv[i], "r");
				if (stream == NULL) {
					print_error(filename);
					++status;
				}
			}
			else {
				stream = NULL;
			}

			if (stream != NULL) {
				if (flags) {
					status += cat(stream, filename, flags);
				}
				else {
					status += fastcat(stream, filename);
				}
				if (stream == stdin) {
					clearerr(stdin);
				}
				else {
					fclose(stream);
				}
			}
		}
	}

	return (status ? 1 : 0);
}

static int not_files_in_argv(char **argv)
{
	int status = 1;
	char *arg = *++argv;

	while (status && arg != NULL) {
		if (arg[0] != '-') {
			status = 0;
		}
		else if (arg[1] == '\0') {
			status = 0;
		}
		arg = *++argv;
	}
	
	return (status);
}

static int cat(FILE *stream, const char *filename, int flags)
{
	static int itline = 0;
	static int skip_line = 0;
	static size_t num = 0;
	int status = 0;
	int c;

	while ((c = getc(stream)) != EOF && !ferror(stdout)) {
		if (c != '\n') {
			if ((flags & NFLAG) && (itline == 0)) {
				printf("%6zu\t", ++num);
			}
			itline = 1;
			if (c == '\t') {
				if (flags & TFLAG) {
					printf("^I");
				}
				else {
					putchar('\t');
				}
			}
			else if (flags & VFLAG) {
				if (c & 0x80) {
					printf("M-");
					c &= 0x7F;
				}
				if (c < 0x20) {
					printf("^%c", c | 0x40);
				}
				else if ( c == 0x7F) {
					printf("^?");
				}
				else {
					putchar(c);
				}
			}
			else {
				putchar(c);
			}
			skip_line = 0;
		}
		else if (!(skip_line && (flags & SFLAG))) {
			if (itline == 0) {
				skip_line = 1;
				if ((flags & NFLAG) && !(flags & BFLAG)) {
					printf("%6zu\t", ++num);
				} 
			}
			if (flags & EFLAG) {
				putchar('$');
			}
			putchar('\n');
			itline = 0;
		}
	}

	if (ferror(stdout)) {
		print_error("(standart output)");
		status = 1;
	}
	if (ferror(stream)) {
		print_error(filename);
		status = 1;
	}

	return (status);
}

static int fastcat(FILE *stream, const char *filename)
{
	int fd = -1;
	ssize_t offset = 0;
	ssize_t nread = 0;
	ssize_t nwrite = 0;
	size_t buf_size = 1024;
	char buf[buf_size];

	fd = fileno(stream);
	if (fd != -1) {
		while ((nread = read(fd, buf, buf_size)) > 0 && nwrite != -1) {
			offset = 0;
			do {
				nwrite = write(STDOUT_FILENO, buf + offset, nread);
				if (nwrite < 0) {
					print_error("(standart output)");
				}
				else {
					offset += nwrite;
					nread -= nwrite;
				}
			} while (nread > 0 && nwrite != -1);
		}
		if (nread == -1) {
			print_error(filename);
		}
	}

	return (nread == -1 || nwrite != -1 ? 1 : 0);
}
