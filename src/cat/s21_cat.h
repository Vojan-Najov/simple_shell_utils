#ifndef S21_CAT_H
#define S21_CAT_H

#define _POSIX_C_SOURCE 200809L

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

#define ERROR 0x40
#define HELP 0x80
#define BUFFER_SIZE 1024

#define HELP_MESSAGE "Try 's21_cat --help' for more inforation.\n"

int get_options(char **argv);
void print_option_error(int c);
void print_long_option_error(const char *option);
void print_usage(void);
int print_error(const char *tag);

#endif
