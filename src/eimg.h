#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#ifndef EXTERN
#define EXTERN extern
#endif

/*
 *	important parameters
 */
#define	SUBWIDTH 4	// width for chroma sub with one sample taken

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef struct Raw Raw;

struct Raw {
	int w, h;
	uint8 (**v)[3];
};

/*
 *	raw.c
 */
Raw	*png2raw(char *);

/*
 *	yuv.c
 */
void	ccol(uint8 *, int (*)[3]);
void	craw(Raw *, int (*)[3]);
void	subsamp(Raw *);

/*
 *	sdl.c
 */
void	sdlinit(int, int);
void	raw2sdl(Raw *);

/*
 *	util.c
 */
void	errorf(char *, ...);
void	*emalloc(size_t);

extern	int		c2yuv[3][3], c2rgb[3][3];

EXTERN	SDL_Window	*win;
EXTERN	SDL_Renderer	*rnd;
EXTERN	SDL_Surface	*scr;
EXTERN	char		*errprefix;
