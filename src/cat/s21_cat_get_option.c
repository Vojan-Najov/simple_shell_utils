#include "s21_cat.h"
#include <string.h>
#include <stdlib.h>

int get_options(char **argv)
{
	int flags = 0;
	char *arg = NULL;
	char c = '\0';

	while ((arg = *++argv))
	{
		if (arg[0] == '-' && arg[1] != '-')
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
						exit(EXIT_FAILURE);
				}
			}
		}
		else if (arg[0] == '-' && arg[1] == '-')
		{
			arg += 2;
			if (strcmp(arg, "show-all") == 0)
			{
				flags |= VFLAG | EFLAG | TFLAG;
			}
			else if (strcmp(arg, "number-nonblank") == 0)
			{
				flags |= BFLAG | NFLAG;
			}
			else if (strcmp(arg, "show-ends") == 0)
			{
				flags |= EFLAG;
			}
			else if (strcmp(arg, "number") == 0)
			{
				flags |= NFLAG;
			}
			else if (strcmp(arg, "squeeze-blank") == 0)
			{
				flags |= SFLAG;
			}
			else if (strcmp(arg, "show-tabs") == 0)
			{
				flags |= TFLAG;
			}
			else if (strcmp(arg, "show-nonprinting") == 0)
			{
				flags |= VFLAG;
			}
			else if (strcmp(arg, "help") == 0)
			{
				print_usage();
			}
			else
			{
				print_long_option_error(arg - 2);
				exit(EXIT_FAILURE);
			}
		}
	}

	return flags;
}
