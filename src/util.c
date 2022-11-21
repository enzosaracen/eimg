#include "eimg.h"

void errorf(char *fmt, ...)
{
	va_list ap;

	if(errprefix != NULL)
		fprintf(stderr, "%s: ", errprefix);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

void *emalloc(size_t n)
{
	void *p;

	p = malloc(n);
	if(p == NULL)
		errorf("malloc: out of memory");
	return p;
}

int ebound(int v, int l)
{
	if(v >= l)
		return l-1;
	if(v < 0)
		return 0;
	return v;
}

char efgetc(FILE *fp)
{
	char c;

	c = fgetc(fp);
	if(feof(fp))
		errorf("fgetc eof");
	return c;
}
