#ifndef S21_GREP_H
#define S21_GREP_H

#define EFLAG 0x01
#define IFLAG 0x02
#define VFLAG 0x04
#define CFLAG 0x08
#define LFLAG 0x10
#define NFLAG 0x20
#define HFLAG 0x40
#define SFLAG 0x80
#define FFLAG 0x100
#define OFLAG 0x200
#define ERROR 0xffff

typedef struct string_list {
	char *content;
	struct string_list *next;
} t_string_list;

int string_list_push_back(t_string_list **head, char *content, int make_copy);

void string_list_clear(t_string_list *head);

int print_error(const char *tag);

int get_options(int argc, char *argv[]);



#endif
