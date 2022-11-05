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

typedef struct strlist {
	char *content;
	int need_free;
	struct strlist *next;
} t_strlist;

int
strlist_push_back(t_strlist **head, char *content, int make_copy);
void
strlist_clear(t_strlist *head);
int
print_error(const char *tag);
int
get_options(int argc, char *argv[], t_strlist **template, t_strlist **filename);



#endif
