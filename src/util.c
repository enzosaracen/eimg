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
