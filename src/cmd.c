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
	srcfile = argv[1];
	r = png2raw(srcfile);
	dctinit();
	initvli();

	/*fout = fopen("out", "w");
	wrbits(2, 4);
	wrbits(2000, 12);
	wrbflush();
	fclose(fout);
	fin = fopen("out", "r");
	a = rdbits(4);
	b = rdbits(12);
	printf("%d %d\n", a, b);*/
	test(r, "out.eimg");
}
