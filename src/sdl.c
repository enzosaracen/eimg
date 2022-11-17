#include "eimg.h"

void sdlinit(int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("eimg", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
	scr = SDL_GetWindowSurface(win);
}

void raw2sdl(Raw *r)
{
	int i, j;

	sdlinit(r->w, r->h);
	for(i = 0; i < r->h; i++)
		for(j = 0; j < r->w; j++)
			((uint32*)scr->pixels)[i*r->w+j] = SDL_MapRGB(scr->format, r->v[i][j][0], r->v[i][j][1], r->v[i][j][2]);
	SDL_UnlockSurface(scr);
	SDL_UpdateWindowSurface(win);
}
