#include "s21_grep.h"
#include <stddef.h>

int main(int argc, char *argv[])
{
	int flags = 0;
	int status = 0;
	t_strlist *template = NULL;
	t_strlist *filename = NULL;

	flags = get_options(argc, argv, &template, &filename);

	if (flags != ERROR) {
		for (t_strlist *fn = filename; fn; fn = fn->next) {
			// open 		
		}
	}
	else {
		status = 1;
	}

	strlist_clear(template);
	strlist_clear(filename);

	return (status);
}
