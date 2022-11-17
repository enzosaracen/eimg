#define EXTERN
#include "eimg.h"

int main(int argc, char **argv)
{
	int i, j;
	Raw *r;

	if(argc != 2)
		return 1;
	srand(time(NULL));
	r = png2raw(argv[1]);
	subsamp(r);
	dctraw(r);
	craw(r, c2rgb);
	raw2sdl(r);
	getchar();
}
