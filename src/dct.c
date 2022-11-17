#include "eimg.h"

double dctab[DCTW][DCTW];

int q1tab[DCTW][DCTW] = {
	{16, 11, 10, 16, 24, 40, 51, 61},
	{12, 12, 14, 19, 26, 58, 60, 55},
	{14, 13, 16, 24, 40, 57, 69, 56},
	{14, 17, 22, 29, 51, 87, 80, 62},
	{18, 22, 37, 56, 68, 109, 103, 77},
	{24, 35, 55, 64, 81, 104, 113, 92},
	{49, 64, 78, 87, 103, 121, 120, 101},
	{72, 92, 95, 98, 112, 100, 103, 99},
};

void dctinit(void)
{
	double x, y;

	#define COEF(v) (((v) == 0) ? (1.0/sqrt(2.0)) : 1.0)

	for(x = 0; x < DCTW; x++)
		for(y = 0; y < DCTW; y++)
			dctab[(int)x][(int)y] = cos(((2.0*x + 1)*y*M_PI)/(2.0*DCTW))*COEF(x)*COEF(y);
}

void dct(int (*img)[DCTW], int (*wts)[DCTW])
{
	int i, j, x, y;
	double tmp;

	for(i = 0; i < DCTW; i++)
		for(j = 0; j < DCTW; j++) {
			tmp = 0;
			for(x = 0; x < DCTW; x++)
				for(y = 0; y < DCTW; y++)
					tmp += dctab[x][i]*dctab[y][j]*img[y][x];
			wts[i][j] = round(tmp/sqrt(2*DCTW));
		}
}

void idct(int (*img)[DCTW], int (*wts)[DCTW])
{
	int i, j, x, y;
	double tmp;

	for(i = 0; i < DCTW; i++)	
		for(j = 0; j < DCTW; j++) {
			tmp = 0;
			for(x = 0; x < DCTW; x++)
				for(y = 0; y < DCTW; y++)
					tmp += dctab[i][x]*dctab[j][y]*wts[y][x];
			if(j == 0 || i == 0)
				tmp *= 2.0;
			img[i][j] = round(tmp/sqrt(2*DCTW));
		}
}

void quant(int (*wts)[DCTW], int (*qtab)[DCTW])
{
	int i, j;

	for(i = 0; i < DCTW; i++)
		for(j = 0; j < DCTW; j++)
			wts[i][j] /= qtab[i][j]*QMOD;
}

void dequant(int (*wts)[DCTW], int (*qtab)[DCTW])
{
	int i, j;

	for(i = 0; i < DCTW; i++)
		for(j = 0; j < DCTW; j++)
			wts[i][j] *= qtab[i][j]*QMOD;
}

void dctraw(Raw *r)
{
	int i, j, x, y, wts[DCTW][DCTW], img[DCTW][DCTW];

	dctinit();
	/*memset(img, 255, DCTW*DCTW*sizeof(int));
	dct(img, wts);
	for(i = 0; i < DCTW; i++) {
		for(j = 0; j < DCTW; j++)
			printf("%d\t", wts[i][j]);
		printf("\n");
	}*/
	for(i = 0; i < r->h-DCTW; i += DCTW)
		for(j = 0; j < r->w-DCTW; j += DCTW) {
			for(y = 0; y < DCTW; y++)
				for(x = 0; x < DCTW; x++)	
					img[y][x] = r->v[i+y][j+x][0];
			dct(img, wts);
			//quant(wts, q1tab);
			//dequant(wts, q1tab);
			idct(img, wts);
			for(y = 0; y < DCTW; y++)
				for(x = 0; x < DCTW; x++)
					r->v[i+y][j+x][0] = img[y][x];
		}
}
