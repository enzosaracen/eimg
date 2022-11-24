#include "eimg.h"

int	ndcpl, nacpl;
uint16	*dcplist, *acplist;
int	dcpfreq[0xffff], acpfreq[0xffff];

void initvli(void)
{
	int i, j, len, prev;

	prev = 0;
	for(i = 0; i < VLIMAX; i++) {
		len = ipow(2, i);
		vlitab[i].v = emalloc(2*len*sizeof(int));
		len += prev;
		for(j = -len; j < -prev; j++)
			vlitab[i].v[vlitab[i].n++] = j;
		for(j = prev+1; j <= len; j++)
			vlitab[i].v[vlitab[i].n++] = j;
		prev = len;
	}
}

int vlisiz(int v)
{
	if(v < 0)
		v = -v;
	return log2(v*2);
}

uint16 vlibits(int v)
{
	if(v < 0)
		v += ipow(2, vlisiz(v))-1;
	return v & 0xffff;
}

void mkdcpair(int v)
{
	static int prev;
	uint16 r;

	r = vlisiz(v-prev) & 0xf;
	r |= vlibits(v-prev) << 4;
	dcplist[ndcpl++] = r;
	dcpfreq[r]++;
	prev = v;
}

void mkacpair(int v, int n0)
{
	uint16 r;

	if(v == 0)
		r = 0;
	else {
		r = n0 & 0xf;
		r |= (vlisiz(v) & 0xf) << 4;
		r |= vlibits(v) << 8;
	}
	acplist[nacpl++] = r;
	acpfreq[r]++;
}

int rddcpair(void)
{
	int v;
	static int prev;
	uint16 siz;

	siz = rdbits(4);
	if(siz == 0)
		v = 0;
	else
		v = vlitab[siz-1].v[rdbits(siz)];
	prev = v + prev;
	return prev;
}

int rdacpair(int reset)
{
	static int n0, siz;
	uint8 b;

	if(reset) {
		n0 = siz = 0;
		return 0;
	}
	if(n0 == 0) {
		b = rdbits(8);
		n0 = b & 0xf;
		siz = (b >> 4) & 0xf;
		if(n0 == 15 && siz == 0)
			n0 = 16;
		else if(n0 == 0 && siz == 0)
			n0 = 63;
		n0++;
	}
	if(n0 > 1) {
		n0--;
		return 0;
	} else if(n0 == 1) {
		n0 = 0;
		return vlitab[siz-1].v[rdbits(siz)];
	}
	errorf("rdacpair: how did we get here?");
	return 0;
}

void wrdcoef(Wts *w)
{
	int i, j;

	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			mkdcpair(w->y[i][j][0][0]);
}

void wracoef(int (*w)[DCTW])
{
	int x, y, ix, iy, cx, cy, n0;

	iy = -1;
	x = n0 = 0;
	y = ix = 1;
	for(;;) {
		if(w[y][x] == 0) {
			n0++;
			if(x == DCTW-1 && y == DCTW-1)
				mkacpair(0, 0);
		} else {
			mkacpair(w[y][x], n0);
			n0 = 0;
		}
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

void rddcoef(Wts *w)
{
	int i, j;

	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			w->y[i][j][0][0] = rddcpair();
}

void rdacoef(int (*w)[DCTW])
{
	int x, y, ix, iy, cx, cy;

	iy = -1;
	x = 0;
	y = ix = 1;
	for(;;) {
		w[y][x] = rdacpair(0);
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
	rdacpair(1);
}

void wts2file(Wts *w)
{
	int i, j, k;

	dcplist = emalloc((w->h*w->yw + w->h*w->uw)*sizeof(uint16));
	acplist = emalloc((w->h*w->yw + w->h*w->uw)*DCTW*DCTW*sizeof(uint16));

	wrdcoef(w);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			wracoef(w->y[i][j]);
	for(k = 0; k < 2; k++)
		for(i = 0; i < w->h; i++)
			for(j = 0; j < w->uw; j++)
				wracoef(w->uv[i][j][k]);
	for(i = 0; i < ndcpl; i++) {
		wrbits(dcplist[i] & 0xf, 4);
		wrbits(dcplist[i] >> 4, dcplist[i] & 0xf);
	}
	for(i = 0; i < nacpl; i++) {
		wrbits(acplist[i] & 0xff, 8);
		wrbits(acplist[i] >> 8, (acplist[i] >> 4) & 0xf);
	}
	wrbflush();
}

void file2wts(Wts *w)
{
	int i, j, k;

	rddcoef(w);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			rdacoef(w->y[i][j]);
	for(k = 0; k < 2; k++)
		for(i = 0; i < w->h; i++)
			for(j = 0; j < w->uw; j++)
				rdacoef(w->uv[i][j][k]);
}
