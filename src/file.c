#include "eimg.h"

FILE *fout, *fin;

void vlqw(int32_t v)
{
	int i, sign, cont;
	unsigned char b;

	sign = (v & 0x80000000) != 0;
	if(sign) {
		v = -v;
		if(v < 0)
			errorf("negative limit for vlqw, qmod likely zero");
	}
	b = v & 0x3f;
	cont = v > b;
	fputc(b | (sign << 6) | (cont << 7), fout);
	for(i = 6; cont; i = 7) {
		v >>= i;
		b = v & 0x7f;
		cont = v > b;
		fputc(b | (cont << 7), fout);
	}
}

int32_t vlqr(void)
{
	int i, cont, sign;
	int32_t v, b;

	v = 0;
	b = efgetc(fin);
	sign = (b & 0x40) ? -1 : 1;
	cont = b & 0x80;
	v |= b & 0x3f;
	for(i = 6; cont; i += 7) {
		b = efgetc(fin);
		v |= (b & 0x7f) << i;
		cont = b & 0x80;
	}
	return v*sign;
}

void test(Raw *r, char *fname)
{
	double tend, tstart, comprate;
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

	fin = fopen(fname, "r");
	if(fin == NULL)
		errorf("decode: cannot open %s", fname);
	printf("\n\n--------- decoding ---------\n");
	BENCH(file2wts(w), "decoding weights");
	BENCH(idctyuv(yuv, w), "inverse dct'ing weights into yuv");
	BENCH(yuv2raw(yuv, r), "yuv back to raw");
	BENCH(craw(r, c2rgb), "raw back to rgb");
	BENCH(raw2sdl(r), "raw into SDL");

	fseek(fin, 0, SEEK_END);
	comprate = 1.0 / ftell(fin);
	fclose(fin);
	fin = fopen(srcfile, "r");
	fseek(fin, 0, SEEK_END);
	comprate *= ftell(fin);
	fclose(fin);
	printf("\ncompression:\tx%.3f\n", comprate);

	getchar();
}
