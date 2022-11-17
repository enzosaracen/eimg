#include "eimg.h"

int c2yuv[3][3] = {
	{66,	129,	25},
	{-38,	-74,	112},
	{112,	-94,	-18},
};

int c2rgb[3][3] = {
	{298,	0,	409},
	{298,	-100,	-208},
	{298,	516,	0},
};

void ccol(uint8 *cur, int (*tab)[3])
{
	int i, j, old[3], sum;

	old[0] = cur[0];
	old[1] = cur[1];
	old[2] = cur[2];
	if(tab == c2rgb) {
		old[0] -= 16;
		old[1] -= 128;
		old[2] -= 128;
	}
	for(i = 0; i < 3; i++) {
		sum = 0;
		for(j = 0; j < 3; j++)
			sum += tab[i][j]*old[j];
		sum = (sum + 128) >> 8;
		if(tab == c2yuv)
			sum += (i == 0) ? 16 : 128;
		if(sum < 0)
			sum = 0;
		else if(sum > 255)
			sum = 255;
		cur[i] = sum;
	}
}

void craw(Raw *r, int (*tab)[3])
{
	int i, j;

	for(i = 0; i < r->h; i++)
		for(j = 0; j < r->w; j++)
			ccol(r->v[i][j], tab);
}

void subsamp(Raw *r)
{
	int i, j, k, width, au, av;

	for(j = 0; j < r->w; j += SUBW) {
		if(r->w-j < SUBW)
			width = r->w-j;
		else
			width = SUBW;
		for(i = 0; i < r->h; i++) {
			au = av = 0;
			for(k = 0; k < width; k++) {
				ccol(r->v[i][j+k], c2yuv);
				au += r->v[i][j+k][1];
				av += r->v[i][j+k][2];
			}
			au /= width;
			av /= width;
			for(k = 0; k < width; k++) {
				r->v[i][j+k][1] = au;
				r->v[i][j+k][2] = av;
			}
		}
	}
}
