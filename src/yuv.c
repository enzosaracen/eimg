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

void yuv2raw(Yuv *yuv, Raw *r)
{
	int i, j, k, width, uvi;

	for(j = 0; j < r->w; j += SUBW) {
		if(r->w-j < SUBW)
			width = r->w-j;
		else
			width = SUBW;
		uvi = UVX(j);
		for(i = 0; i < r->h; i++) {
			for(k = 0; k < width; k++) {
				r->v[i][j+k][0] = yuv->y[i][j+k];
				r->v[i][j+k][1] = yuv->uv[i][uvi][0];
				r->v[i][j+k][2] = yuv->uv[i][uvi][1];
			}
		}
	}
}

Yuv *subsamp(Raw *r)
{
	int i, j, k, width, au, av;
	Yuv *yuv;

	yuv = emalloc(sizeof(Yuv));
	yuv->h = r->h + (DCTW - (r->h % DCTW));
	yuv->yw = r->w + (DCTW - (r->w % DCTW));
	yuv->y = emalloc(yuv->h*sizeof(*yuv->y));
	yuv->uw = UVX(r->w) + (DCTW - (UVX(r->w) % DCTW));
	yuv->uv = emalloc(yuv->h*sizeof(*yuv->y));
	for(i = 0; i < yuv->h; i++) {
		yuv->y[i] = emalloc(yuv->yw*sizeof(**yuv->y));
		yuv->uv[i] = emalloc(yuv->uw*sizeof(**yuv->uv));
	}
	for(j = 0; j < r->w; j += SUBW) {
		if(r->w-j < SUBW)
			width = r->w-j;
		else
			width = SUBW;
		for(i = 0; i < r->h; i++) {
			au = av = 0;
			for(k = 0; k < width; k++) {
				ccol(r->v[i][j+k], c2yuv);
				yuv->y[i][j+k] = r->v[i][j+k][0];
				au += r->v[i][j+k][1];
				av += r->v[i][j+k][2];
			}
			yuv->uv[i][UVX(j)][0] = au / width;
			yuv->uv[i][UVX(j)][1] = av / width;
		}
	}

	for(i = 0; i < r->h; i++) {
		for(j = r->w; j < yuv->yw; j++)
			yuv->y[i][j] = yuv->y[i][r->w-1];
		for(j = UVX(r->w); j < yuv->uw; j++) {
			yuv->uv[i][j][0] = yuv->uv[i][UVX(r->w-SUBW)][0];
			yuv->uv[i][j][1] = yuv->uv[i][UVX(r->w-SUBW)][1];
		}
	}
	for(i = r->h; i < yuv->h; i++) {
		for(j = 0; j < r->w; j++)
			yuv->y[i][j] = yuv->y[r->h-1][j];
		for(j = 0; j < UVX(r->w); j++) {
			yuv->uv[i][j][0] = yuv->uv[r->h-1][j][0];
			yuv->uv[i][j][1] = yuv->uv[r->h-1][j][1];
		}
	}
	for(i = r->h; i < yuv->h; i++) {
		for(j = r->w; j < yuv->yw; j++)
			yuv->y[i][j] = yuv->y[r->h-1][r->w-1];
		for(j = UVX(r->w); j < yuv->uw; j++) {
			yuv->uv[i][j][0] = yuv->uv[r->h-1][UVX(r->w-SUBW)][0];
			yuv->uv[i][j][1] = yuv->uv[r->h-1][UVX(r->w-SUBW)][1];
		}
	}
	return yuv;
}
