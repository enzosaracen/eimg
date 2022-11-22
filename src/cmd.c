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
	test(r, "out.eimg");
}
