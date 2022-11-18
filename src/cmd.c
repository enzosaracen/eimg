#define EXTERN
#include "eimg.h"

int main(int argc, char **argv)
{
	Raw *r;

	if(argc != 2)
		return 1;
	srand(time(NULL));
	r = png2raw(argv[1]);
	dctinit();
	encode(r, "out.eimg");
	return 0;
}
