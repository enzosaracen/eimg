#define EXTERN
#include "eimg.h"

int main(int argc, char **argv)
{

	#define T ((double)clock()/CLOCKS_PER_SEC)

	int i, j;
	double start, end;
	Raw *r;
	Yuv *yuv;

	if(argc != 2)
		return 1;
	srand(time(NULL));
	r = png2raw(argv[1]);

	errprefix = "subsampling raw";
	start = T;
	yuv = subsamp(r);
	end = T;
	printf("%fs:\t%s\n", end-start, errprefix);

	errprefix = "dct'ing yuv";
	start = T;
	dctyuv(yuv);
	end = T;
	printf("%fs:\t%s\n", end-start, errprefix);

	errprefix = "yuv back to raw";
	start = T;
	yuv2raw(yuv, r);
	end = T;
	printf("%fs:\t%s\n", end-start, errprefix);

	errprefix = "raw back to rgb";
	start = T;
	craw(r, c2rgb);
	end = T;
	printf("%fs:\t%s\n", end-start, errprefix);

	errprefix = "raw into SDL";
	start = T;
	raw2sdl(r);
	end = T;
	printf("%fs:\t%s\n", end-start, errprefix);
	getchar();
}
