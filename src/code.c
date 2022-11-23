#include "eimg.h"

void wrdcoef(Wts *w)
{
	int i, j, k, prev;

	prev = 0;
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++) {
			vlqw(w->y[i][j][0][0] - prev);
			prev = w->y[i][j][0][0];
		}
	for(k = 0; k < 2; k++) {
		prev = 0;
		for(i = 0; i < w->uw; i++) {
			vlqw(w->uv[i][j][k][0][0] - prev);
			prev = w->uv[i][j][k][0][0];
		}
	}
}

void rddcoef(Wts *w)
{
	int i, j, k, prev;

	prev = 0;
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++) {
			w->y[i][j][0][0] = vlqr() + prev;
			prev = w->y[i][j][0][0];
		}
	for(k = 0; k < 2; k++) {
		prev = 0;
		for(i = 0; i < w->uw; i++) {
			w->uv[i][j][k][0][0] = vlqr() + prev;
			prev = w->uv[i][j][k][0][0];
		}
	}
}

void wrw(int v)
{
	static int n0;

	if(v == 0) {
		n0++; 
		return;
	} else if(n0 > 0) {
		vlqw(0);
		vlqw(n0);
		n0 = 0;
	}
	vlqw(v);
}

int rdw(void)
{
	static int n0;
	int32_t v;

	if(n0 > 0) {
		n0--;	
		return 0;
	}
	v = vlqr();
	if(v == 0)
		n0 = vlqr()-1;
	return v;
}

void wrwts(int (*w)[DCTW])
{
	int x, y, ix, iy, cx, cy;

	iy = -1;
	x = 0;
	y = ix = 1;
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

	wrdcoef(w);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			wrwts(w->y[i][j]);
	for(k = 0; k < 2; k++)
		for(i = 0; i < w->h; i++)
			for(j = 0; j < w->uw; j++)
				wrwts(w->uv[i][j][k]);
}

void file2wts(Wts *w)
{
	int i, j, k;

	rddcoef(w);
	for(i = 0; i < w->h; i++)
		for(j = 0; j < w->yw; j++)
			rdwts(w->y[i][j]);
	for(k = 0; k < 1; k++)
		for(i = 0; i < w->h; i++)
			for(j = 0; j < w->uw; j++)
				rdwts(w->uv[i][j][k]);
}
