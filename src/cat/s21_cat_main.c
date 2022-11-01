#include "s21_cat.h"

int main(int argc, char **argv)
{
	int flags = 0;
	int fstdin = 0;
	FILE *stream = NULL;
	int ret_value = 0;

	flags = get_options(argv);

	if (argc == 1)
	{
		argc = 2;
		fstdin = 1;
	}

	for (int i = 1; i < argc; ++i)
	{
		if (fstdin || (argv[i][0] == '-' && argv[i][0] == '\0'))
		{
			stream = stdin;
		}
		else if (argv[i][0] != '-')
		{
			stream = fopen(argv[i], "r");
			if (!stream)
			{
				print_file_error(argv[i]);
				ret_value += 1;
				continue;
			}
		}
		else
		{
			continue;
		}

		if (flags)
		{
			ret_value += cat(stream, flags);
		}
		else
		{
			ret_value += fastcat(stream);
		}

		if (stream != stdin)
		{
			if (fclose(stream) == EOF)
			{
				print_file_error(argv[i]);
				ret_value += 1;
			}
		}
	}

	return ret_value ? 1 : 0;
}
