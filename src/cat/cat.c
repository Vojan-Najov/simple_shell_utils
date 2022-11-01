#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
-b or --number-nonbank
Number the non-blank output lines, starting at 1.
*/
#define BFLAG 0x01

/*
-e
Display non-printing characters (see the -v option), and display a dollar
sign (`$') at the end of each line.
-E
Display ('$') at the end of each line.
*/
#define EFLAG 0x02

/*
-n or --number
Number the output lines, starting at 1.
*/
#define NFLAG 0x04

/*
-s
Squeeze multiple adjacent empty lines, causing the output to be single
spaced.
*/
#define SFLAG 0x08

/*
-t
Display non-printing characters (see the -v option), and display tab
characters as `^I'
-T
Display tab characters as ^I
*/
#define TFLAG 0x10

/*
-v
Display non-printing characters so they are visible.
Control characters print as `^X' for control-X;
the delete character (octal 0177) prints as`^?'
Non- ASCII characters (with the high bit set) are printed as `M-'
(for meta) followed by the character for the low 7 bits.
*/
#define VFLAG 0x20

#define OPTION_ERROR_MESSAGE "s21_cat: invalid option -- "
#define LONG_OPTION_ERROR_MESSAGE "s21_cat: unrecognized option '"
#define HELP_MESSAGE "Try 's21_cat --help' for more inforation.\n"
#define USAGE_MESSAGE                                                       \
                                                                            \
  "Usage: cat [OPTION]... [FILE]...\n"                                      \
  "Concatenate FILE(s) to standard output.\n"                               \
  "\nWith no FILE, or when FILE is -, read standard input.\n"               \
  "\n  -A, --show-all           equivalent to -vET\n"                       \
  "  -b, --number-nonblank    number nonempty output lines, overrides -n\n" \
  "  -e                       equivalent to -vE\n"                          \
  "  -E, --show-ends          display $ at end of each line\n"              \
  "  -n, --number             number all output lines\n"                    \
  "  -s, --squeeze-blank      suppress repeated empty output lines\n"       \
  "  -t                       equivalent to -vT\n"                          \
  "  -T, --show-tabs          display TAB characters as ^I\n"               \
  "  -u                       (ignored)\n"                                  \
  "  -v, --show-nonprinting   "                                             \
  "use ^ and M- notation, except for LFD and TAB\n"                         \
  "      --help        display this help and exit\n"

int get_options(char **argv);
int cat(FILE *f, int flags);
int fastcat(FILE *stream);
void print_option_error(int c);
void print_long_option_error(const char *option);
void print_usage(void);

int main(int argc, char **argv) {
  int flags = 0;
  int stdin_flag = 0;
  FILE *f;
  int retval = 0;

  flags = get_options(argv);
  if (argc == 1) {
    argc = 2;
    stdin_flag = 1;
  }
  for (int i = 1; i < argc; ++i) {
    if (stdin_flag || (argv[i][0] == '-' && argv[i][1] == '\0'))
      f = stdin;
    else if (argv[i][0] != '-') {
      if ((f = fopen(argv[i], "r")) == NULL) {
        write(STDERR_FILENO, "s21_cat: ", sizeof("s21_cat: ") - 1);
        perror(argv[i]);
        retval = EXIT_FAILURE;
        continue;
      }
    } else
      continue;
    // (void) fileno(f) ;
    // check_redirection(f);
    if (!flags)
      fastcat(f);  // fileno(f));
    else
      cat(f, flags);
    if (f != stdin) fclose(f);
  }

  return retval;
}
/*
void check_redirection(FILE *stream)
{

}
*/
int get_options(char **argv) {
  char *arg = NULL;
  char c = '\0';
  int flags = 0;

  while ((arg = *++argv)) {
    if (arg[0] == '-' && arg[1] != '-') {
      while ((c = *++arg)) {
        switch (c) {
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
            exit(EXIT_FAILURE);
        }
      }
    } else if (arg[0] == '-' && arg[1] == '-') {
      arg += 2;
      if (strcmp(arg, "show-all") == 0)
        flags |= VFLAG | EFLAG | TFLAG;
      else if (strcmp(arg, "number-nonblank") == 0)
        flags |= BFLAG | NFLAG;
      else if (strcmp(arg, "show-ends") == 0)
        flags |= EFLAG;
      else if (strcmp(arg, "number") == 0)
        flags |= NFLAG;
      else if (strcmp(arg, "squeeze-blank") == 0)
        flags |= SFLAG;
      else if (strcmp(arg, "show-tabs") == 0)
        flags |= TFLAG;
      else if (strcmp(arg, "show-nonprinting") == 0)
        flags |= VFLAG;
      else if (strcmp(arg, "help") == 0)
        print_usage();
      else {
        print_long_option_error(arg - 2);
        exit(EXIT_FAILURE);
      }
    }
  }

  return flags;
}

int cat(FILE *f, int flags) {
  static int itline = 0;
  static int spaced = 0;
  static int num = 0;
  int c = EOF;

  while ((c = getc(f)) != EOF) {
    if (c == '\n') {
      if (itline == 0) {
        if ((flags & SFLAG) && spaced) continue;
        spaced = 1;
      }
      if ((flags & NFLAG) && !(flags & BFLAG) && itline == 0) {
        printf("%6d\t", ++num);
      }
      if (flags & EFLAG) putchar('$');
      putchar('\n');
      itline = 0;
      continue;
    }
    if (flags & NFLAG && itline == 0) printf("%6d\t", ++num);
    itline = 1;
    if (flags & VFLAG) {
      if (!(flags & TFLAG) && c == '\t')
        putchar('\t');
      else {
        if (c > 0177) {
          printf("M-");
          c &= 0177;
        }
        if (c < ' ')
          printf("^%c", c + '@');
        else if (c == 0177)
          printf("^?");
        else
          putchar(c);
      }
    } else if (flags & TFLAG && c == '\t') {
      printf("^I");
    } else
      putchar(c);
    spaced = 0;
  }
  return 0;
}

int fastcat(FILE *stream) {
  // int retval = 0;
  int BUF_SIZE = 1024;
  char buf[BUF_SIZE];
  int nread = 0;
  int nwrite = 0;
  int offset = 0;

  while (!feof(stream)) {
    nread = fread(buf, sizeof(char), BUF_SIZE, stream);
    if (ferror(stream)) {
      perror("cat: read error");
      return 1;
    }
    offset = 0;
    do {
      nwrite = fwrite(buf + offset, sizeof(char), nread, stdout);
      if (ferror(stdout)) {
        perror("s21_cat: write error");
        return 1;
      }
      offset += nwrite;
    } while ((nread -= nwrite) > 0);
  }

  return 0;
}
/*
int fastcat(int fd)
{
        //int retval = 0;
        int BUF_SIZE = 10;
        char buf[BUF_SIZE];
        int nread = 0;
        int nwrite = 0;
        int offset = 0;

        while ((nread = read(fd, buf, BUF_SIZE)) > 0)
        {
                offset = 0;
                do
                {
                        nwrite = write(STDOUT_FILENO, buf + offset, nread);
                        offset += nwrite;
                } while ((nread -= nwrite) > 0);
        }

        return 0;
}
*/

void print_option_error(int c) {
  char buf[] = "'c'\n";

  buf[1] = c;
  write(STDERR_FILENO, OPTION_ERROR_MESSAGE, sizeof(OPTION_ERROR_MESSAGE) - 1);
  write(STDERR_FILENO, buf, sizeof(buf) - 1);
  write(STDERR_FILENO, HELP_MESSAGE, sizeof(HELP_MESSAGE) - 1);
}
void print_long_option_error(const char *option) {
  char buf[] = "'\n";

  write(STDERR_FILENO, LONG_OPTION_ERROR_MESSAGE,
        sizeof(LONG_OPTION_ERROR_MESSAGE) - 1);
  write(STDERR_FILENO, option, strlen(option));
  write(STDERR_FILENO, buf, sizeof(buf) - 1);
}

void print_usage(void) {
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

  write(STDOUT_FILENO, usage, sizeof(usage) - 1);
}
