#include "eimg.h"

int wrbuf[8], nwrb;
int rdbuf[8], nrdb = 8;

void wrb(int v)
{
	int i;
	uint8 b;

	wrbuf[nwrb++] = v;
	if(nwrb >= 8) {
		b = 0;
		for(i = 0; i < 8; i++)
			b |= wrbuf[i] << i;
		fputc(b, fout);
		nwrb = 0;
	}
}

int rdb(void)
{
	int i;
	uint8 b;

	if(nrdb >= 8) {
		b = fgetc(fin);
		for(i = 0; i < 8; i++)
			rdbuf[i] = (b >> i) & 1;
		nrdb = 0;
	}
	return rdbuf[nrdb++];
}

void wrbits(uint16 v, int n)
{
	int i;

	for(i = 0; i < n; i++)
		wrb((v >> i) & 1);
}

void wrbflush(void)
{
	int i;

	if(nwrb == 0)
		return;
	for(i = nwrb; i < 7; i++)
		wrbuf[i] = 0;
	nwrb = 7;
	wrb(0);
}

uint16 rdbits(int n)
{
	int i;
	uint16 v;

	v = 0;
	for(i = 0; i < n; i++)
		v |= rdb() << i;
	return v;
}
