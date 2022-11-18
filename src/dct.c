#include "eimg.h"

double dctab[DCTW][DCTW], cotab[DCTW][DCTW];

int yq1tab[DCTW][DCTW] = {
	{16, 11, 12, 15, 21, 32, 50, 66},
	{11, 12, 13, 18, 24, 46, 62, 73},
	{12, 13, 16, 23, 38, 56, 73, 75},
	{15, 18, 23, 29, 53, 75, 83, 83},
	{21, 24, 38, 53, 68, 95, 103, 103},
	{32, 46, 56, 75, 95, 104, 117, 117},
	{50, 62, 73, 83, 103, 117, 120, 120},
	{66, 73, 75, 83, 103, 117, 120, 120},
};

int uvq1tab[DCTW][DCTW] = {
	{17, 18, 24, 47, 99, 99, 99, 99},
	{18, 21, 99, 66, 99, 99, 99, 99},
	{24, 26, 56, 99, 99, 99, 99, 99},
	{47, 66, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
};

void dctinit(void)
{
	double x, y;

	#define COEF(v) (((v) == 0) ? (1.0/sqrt(2.0)) : 1.0)

	for(x = 0; x < DCTW; x++)
		for(y = 0; y < DCTW; y++) {
			dctab[(int)x][(int)y] = cos(((2.0*x + 1)*y*M_PI)/(2.0*DCTW));
			cotab[(int)x][(int)y] = COEF(x)*COEF(y);
			cotab[(int)x][(int)y] *= cotab[(int)x][(int)y];
		}
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
					tmp += dctab[x][i]*dctab[y][j]*img[y][x]*cotab[i][j];
			wts[i][j] = round(tmp/sqrt(2*DCTW)*2.0);
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
			img[i][j] = round(tmp/sqrt(2*DCTW)/2.0);
		}
}

void quant(int (*wts)[DCTW], int (*qtab)[DCTW], double mod)
{
	int i, j;

	for(i = 0; i < DCTW; i++)
		for(j = 0; j < DCTW; j++)
			wts[i][j] /= qtab[i][j]*mod;
}

void dequant(int (*wts)[DCTW], int (*qtab)[DCTW], double mod)
{
	int i, j;

	for(i = 0; i < DCTW; i++)
		for(j = 0; j < DCTW; j++)
			wts[i][j] *= qtab[i][j]*mod;
}

void cpywts(int (*dest)[DCTW], int (*src)[DCTW])
{
	int i, j;
	
	for(i = 0; i < DCTW; i++)
		for(j = 0; j < DCTW; j++)
			dest[i][j] = src[i][j];
}

Wts *dctyuv(Yuv *yuv)
{
	int i, j, k, x, y, wts[DCTW][DCTW], img[DCTW][DCTW];
	Wts *w;

	w = emalloc(sizeof(Wts));
	w->h = yuv->h / DCTW;
	w->yw = yuv->yw / DCTW;
	w->y = emalloc(w->h*sizeof(*w->y));
	w->uw = yuv->uw / DCTW;
	w->uv = emalloc(w->h*sizeof(*w->uv));
	for(i = 0; i < w->h; i++) {
		w->y[i] = emalloc(w->yw*sizeof(**w->y));
		w->uv[i] = emalloc(w->uw*sizeof(**w->uv));
	}
	for(i = 0; i < yuv->h; i += DCTW) {
		for(j = 0; j < yuv->uw; j += DCTW) {
			for(k = 0; k < 2; k++) {
				for(y = 0; y < DCTW; y++)
					for(x = 0; x < DCTW; x++)
						img[y][x] = yuv->uv[i+y][j+x][k];
				dct(img, wts);
				quant(wts, uvq1tab, uvqmod);
				cpywts(w->uv[i/DCTW][j/DCTW][k], wts);
			
			}
		}
		for(j = 0; j < yuv->yw; j += DCTW) {
			for(y = 0; y < DCTW; y++)
				for(x = 0; x < DCTW; x++)	
					img[y][x] = yuv->y[i+y][j+x];
			dct(img, wts);
			quant(wts, yq1tab, yqmod);
			cpywts(w->y[i/DCTW][j/DCTW], wts);
		}
	}
	return w;
}

void idctyuv(Yuv *yuv, Wts *w)
{
	int i, j, k, x, y, img[DCTW][DCTW]; 

	for(i = 0; i < w->h; i++) {
		for(k = 0; k < 2; k++)
			for(j = 0; j < w->uw; j++) {
				dequant(w->uv[i][j][k], uvq1tab, uvqmod);
				idct(img, w->uv[i][j][k]);
				for(y = 0; y < DCTW; y++)
					for(x = 0; x < DCTW; x++)
						yuv->uv[i*DCTW+y][j*DCTW+x][k] = img[y][x];
			}
		for(j = 0; j < w->yw; j++) {
			dequant(w->y[i][j], yq1tab, yqmod);
			idct(img, w->y[i][j]);
			for(y = 0; y < DCTW; y++)
				for(x = 0; x < DCTW; x++)
					yuv->y[i*DCTW+y][j*DCTW+x] = img[y][x];
		}
	}
}
