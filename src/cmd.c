#define EXTERN
#include "eimg.h"

int main(int argc, char **argv)
{
	Raw *r;
	double q;

	if(argc < 2)
		return 1;
	if(argc == 3) {
		q = atof(argv[2]);
		yqmod = uvqmod = q;
	} else
		yqmod = uvqmod = 1;
	srand(time(NULL));
	r = png2raw(argv[1]);
	dctinit();

/*	FILE *fp = fopen("testfile", "w");
	vlqw(-2147483647, fp);
	fclose(fp);
	fp = fopen("testfile", "r");
	printf("%d\n", vlqr(fp));*/
	test(r, "out.eimg");
}
