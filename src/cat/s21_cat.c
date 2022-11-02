#include "s21_cat.h"

int cat(FILE *stream, int flags)
{
	static int itline = 0;
	static int skip_line = 0;
	static int num = 0;
	int c = EOF;

	while ((c = getc(stream)) != EOF)
	{
		if (c == '\n')
		{
			if (itline == 0)
			{
				if ((flags & SFLAG) && skip_line)
				{
					continue;
				}
				skip_line = 1;
			}
			if ((flags & NFLAG) && !(flags & BFLAG) && itline == 0)
			{
				printf("%6d\t", ++num);
			}
			if (flags & EFLAG)
			{
				putchar('$');
			}
			putchar('\n');
			itline = 0;
			continue;
		}
		if ((flags & NFLAG) && itline == 0)
		{
			printf("%6d\t", ++num);
		}
		itline = 1;
		if (flags & VFLAG)
		{
			if (!(flags & TFLAG) && c == '\t')
			{
				putchar('\t');
			}
			else
			{
				if (c > 0x7F)
				{
					printf("M-");
					c &= 0x7F;
				}
				if (c < ' ')
				{
					printf("^%c", '@' + c);
				}
				else if (c == 0x7F)
				{
					printf("^?");
				}
				else
				{
					putchar(c);
				}
			}
		}
		else if ((flags & TFLAG) && c == '\t')
		{
			printf("^I");
		}
		else
		{
			putchar(c);
		}
		skip_line = 0;
	}

	if (ferror(stdout))
	{
		perror("s21_cat: write error");
		return 1;
	}

	return 0;
}

int fastcat(FILE *stream)
{
	size_t buf_size = 1024;
	size_t size = sizeof(char);
	char buf[buf_size];
	int nread;
	int nwrite;
	int offset;

	while (!feof(stream))
	{
		nread = fread(buf, size, buf_size, stream);
		if (ferror(stream))
		{
			perror("s21_cat: read error");
			return 1;
		}

		offset = 0;
		do
		{
			nwrite = fwrite(buf + offset, size, nread, stdout);
			if (ferror(stdout))
			{
				perror("s21_cat: write error");
				return 1;
			}
			offset += nwrite;
		} while ((nread -= nwrite) > 0);
	}

	return 0;
}
