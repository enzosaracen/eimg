#include "eimg.h"

void diagwt(char *b, int *nb, int (*wt)[DCTW][DCTW])
{
	int x, y;

	for()
}

Blist *w2blist(Wts *w)
{
	int i, j, nb;
	Blist *b;

	b = emalloc(sizeof(Blist));
	b->n = w->yw*w->h*DCTW*DCTW + w->uw*w->h*DCTW*DCTW;
	b->b = emalloc(b->n);
	nb = 0;
	for(i = 0; i < w->h; i++) {
		for(j = 0; j < w->yw; j++) {
			diagwt(b->b, &nb);
		}
			
	}
}

void encodef(Raw *raw, char *out)
{
	FILE *fout;
	Yuv *yuv;
	Wts *w;


	fout = fopen(out, "w");
	if(fout == NULL)
		errorf("cannot open %s", out);

	yuv = subsamp(raw);
	w = dctyuv(yuv);
	w2blist(w);
}
