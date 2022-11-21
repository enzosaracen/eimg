#include "eimg.h"

FILE *fout, *fin;

void vlqw(int32_t v, FILE *fp)
{
	int i, sign, cont;
	unsigned char b;

	sign = (v & 0x80000000) != 0;
	if(sign)
		v = -v;
	b = v & 0x3f;
	cont = v > b;
	fputc(b | (sign << 6) | (cont << 7), fp);
	for(i = 6; cont; i += 7) {
		v >>= i;
		b = v & 0x7f;
		cont = v > b;
		fputc(b | (cont << 7), fp);
	}
}

int32_t vlqr(FILE *fp)
{
	int i, cont, sign;
	int32_t v, b;

	v = 0;
	b = efgetc(fp);
	sign = (b & 0x40) ? -1 : 1;
	cont = b & 0x80;
	v |= b & 0x3f;
	for(i = 6; cont; i += 7) {
		b = efgetc(fp);
		v |= (b & 0x7f) << i;
		cont = b & 0x80;
	}
	return v*sign;
}

void wrw(int v)
{
	static int total;
	static int n0;

	if(v == 0) {
		n0++; 
		return;
	} else if(n0 > 0) {
		fputc(0, fout);
		vlqw(n0, fout);
		n0 = 0;
	}
	vlqw(v, fout);
}

int rdw(void)
{
	static int total;
	static int n0;
	int32_t v;

	if(n0 > 0 && --n0 > 0)
		return 0;
	v = vlqr(fin);
	if(v == 0)
		n0 = vlqr(fin);
	return v;
}

void wrwts(int (*w)[DCTW])
{
	int x, y, ix, iy, cx, cy;

	iy = -1;
	x = 0;
	y = ix = 1;
	wrw(w[0][0]);
	for(;;) {
		wrw(w[y][x]);
		if(x == DCTW-1 && y == DCTW-1)
			break;
		y += iy;
		x += ix;
		cx = ebound(x, DCTW);
		cy = ebound(y, DCTW);
		if(cx != x || cy != y) {
			ix = -ix;
			iy = -iy;
			if(cx != x && cy != y) {
				x = cx;
				y = cy + iy;
			} else {
				if(x >= DCTW && y >= 0)
					cy = y + 2*iy;
				if(y >= DCTW && x >= 0)
					cx = x + 2*ix;
				x = cx;
				y = cy;
			}
		}
	}
}

void rdwts(int (*w)[DCTW])
{
	int x, y, ix, iy, cx, cy;

	iy = -1;
	x = 0;
	y = ix = 1;
	w[0][0] = rdw();
	for(;;) {
		w[y][x] = rdw();
		if(x == DCTW-1 && y == DCTW-1)
			break;
		y += iy;
		x += ix;
		cx = ebound(x, DCTW);
		cy = ebound(y, DCTW);
		if(cx != x || cy != y) {
			ix = -ix;
			iy = -iy;
			if(cx != x && cy != y) {
				x = cx;
				y = cy + iy;
			} else {
				if(x >= DCTW && y >= 0)
					cy = y + 2*iy;
				if(y >= DCTW && x >= 0)
					cx = x + 2*ix;
				x = cx;
				y = cy;
			}
		}
	}
}

void wts2file(Wts *w)
{
	int i, j, k;

	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			wrwts(w->y[i][j]);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->uw; j++)
			for(k = 0; k < 2; k++)
				wrwts(w->uv[i][j][k]);
}

void file2wts(Wts *w)
{
	int i, j, k;

	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			rdwts(w->y[i][j]);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->uw; j++)
			for(k = 0; k < 1; k++)
				rdwts(w->uv[i][j][k]);
}

void test(Raw *r, char *fname)
{
	int i, j;
	double tend, tstart;
	Yuv *yuv;
	Wts *w;

	fout = fopen(fname, "w");
	if(fout == NULL)
		errorf("encode: cannot open %s", fname);
	printf("--------- encoding ---------\n");
	printf("luma qmod:\t%f\n", yqmod);
	printf("chroma qmod:\t%f\n", uvqmod);
	BENCH(yuv = subsamp(r), "subsampling raw into yuv");
	BENCH(w = dctyuv(yuv), "dct'ing yuv into weights");
	BENCH(wts2file(w), "encoding weights");
	fclose(fout);

	for(i = 0; i < DCTW; i++) {
		for(j = 0; j < DCTW; j++)
			printf("%d\t", w->uv[0][0][0][i][j]);
		printf("\n");
	}

	fin = fopen(fname, "r");
	if(fin == NULL)
		errorf("decode: cannot open %s", fname);
	printf("\n\n--------- decoding ---------\n");
	BENCH(file2wts(w), "decoding weights");

	for(i = 0; i < DCTW; i++) {
		for(j = 0; j < DCTW; j++)
			printf("%d\t", w->uv[0][0][0][i][j]);
		printf("\n");
	}

	BENCH(idctyuv(yuv, w), "inverse dct'ing weights into yuv");
	BENCH(yuv2raw(yuv, r), "yuv back to raw");
	BENCH(craw(r, c2rgb), "raw back to rgb");
	BENCH(raw2sdl(r), "raw into SDL");
	fclose(fin);
	getchar();
}
