#include "eimg.h"

char codechar(int v)
{
	if(v >= 128 || v < -128)
		errorf("codechar: weight value too large, check qmod");
	return (char)v;
}

void codeput(Code *c, int v)
{
	if(c->i != 0)
		if(v == c->val[c->i-1]) {
			c->nval[c->i-1]++;
			return;
		}
	if(c->i >= c->n)
		errorf("codeput: code length overflow");
	c->val[c->i] = codechar(v);
	c->nval[c->i++] = 1;
}

int diagbound(int v)
{
	if(v >= DCTW)
		return DCTW-1;
	if(v < 0)
		return 0;
	return v;
}

void diagwts(Code *c, int (*w)[DCTW])
{
	int x, y, inc;

	x = 0;
	y = inc = 1;
	while(!(x == DCTW-1 && y == DCTW-1)) {
		codeput(c, w[y][x]);
		y += inc;
		x += inc;
		if(diagbound(x) != x || diagbound(y) != y)
			inc = -inc;
		x = diagbound(x);
		y = diagbound(y);
	}
}

Code *wts2code(Wts *w)
{
	int i, j, k;
	Code *c;

	c = emalloc(sizeof(Code));
	c->n = w->yw*w->h*DCTW*DCTW + w->uw*w->h*DCTW*DCTW;
	c->i = 0;
	c->val = emalloc(c->n);
	c->nval = emalloc(c->n*sizeof(int));
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			diagwts(c, w->y[i][j]);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->uw; j++)
			for(k = 0; k < 2; k++)
				diagwts(c, w->uv[i][j][k]);
	return c;
}

Wts *encode(Raw *r, char *out)
{
	int i;
	double tend, tstart;
	FILE *fout;
	Yuv *yuv;
	Wts *w;
	Code *c;

	fout = fopen(out, "w");
	if(fout == NULL)
		errorf("encode: cannot open %s", out);

	printf("--------- encoding ---------\n");
	printf("luma qmod:\t%f\n", yqmod);
	printf("chroma qmod:\t%f\n", uvqmod);
	BENCH(yuv = subsamp(r), "subsampling raw into yuv");
	BENCH(w = dctyuv(yuv), "dct'ing yuv into weights");
	BENCH(c = wts2code(w), "encoding weights");
	printf("w/o code:\t%d\nw/ code:\t%d\ncompression:\t%.2fx\n", c->n, c->i, ((double)c->n) / ((double)c->i));

	printf("\n\n--------- decoding ---------\n");
	BENCH(idctyuv(yuv, w), "inverse dct'ing weights into yuv");
	BENCH(yuv2raw(yuv, r), "yuv back to raw");
	BENCH(craw(r, c2rgb), "raw back to rgb");
	BENCH(raw2sdl(r), "raw into SDL");
	getchar();

}
