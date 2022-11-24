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
#define	SUBW	4	// width for chroma sub with one sample taken
#define DCTW 	8	// dct NxN block width
EXTERN	double yqmod;	// yqtab values multiplied by qmod, higher = lower quality
EXTERN	double uvqmod;	// uvqtab ^

#define VLIMAX	16

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef struct Raw Raw;
typedef struct Yuv Yuv;
typedef struct Wts Wts;

struct Raw {
	int w, h;
	uint8 (**v)[3];
};

struct Yuv {
	int h;
	int yw;
	uint8 **y;
	int uw;
	uint8 (**uv)[2];
};

struct Wts {
	int h;
	int yw;
	int (**y)[DCTW][DCTW];
	int uw;
	int (**uv)[2][DCTW][DCTW];
};

#define UVX(j) ((int)(ceil(((double)(j))/SUBW)))

#define TDIF ((double)clock()/CLOCKS_PER_SEC)

#define BENCH(fn, str) \
	errprefix = str; \
	tstart = TDIF; \
	fn; \
	tend = TDIF; \
	printf("%fs:\t%s\n", tend-tstart, errprefix); \
	errprefix = NULL;

/*
 *	raw.c
 */
Raw	*png2raw(char *);

/*
 *	yuv.c
 */
void	ccol(uint8 *, int (*)[3]);
void	craw(Raw *, int (*)[3]);
void	yuv2raw(Yuv *, Raw *);
Yuv	*subsamp(Raw *);

/*
 *	dct.c
 */
void	dctinit(void);
void	dct(int (*)[DCTW], int(*)[DCTW]);
void	idct(int (*)[DCTW], int(*)[DCTW]);
void	quant(int (*)[DCTW], int(*)[DCTW], double);
void	dequant(int (*)[DCTW], int(*)[DCTW], double);
void	cpywts(int (*)[DCTW], int (*)[DCTW]);
Wts	*dctyuv(Yuv *);
void	idctyuv(Yuv *, Wts *);

/*
 *	file.c
 */
void	test(Raw *, char *);

/*
 *	huff.c
 */
void	initvli(void);
int	vlisiz(int);
uint16	vlibits(int);
void	mkdcpair(int);
void	mkacpair(int, int);
int	rddcpair(void);
int	rdacpair(int);
void	wrdcoef(Wts *);
void	wracoef(int (*)[DCTW]);
void	rddcoef(Wts *);
void	rdacoef(int (*)[DCTW]);
void	wts2file(Wts *);
void	file2wts(Wts *);

/*
 *	bit.c
 */
void	wrb(int);
int	rdb(void);
void	wrbits(uint16, int);
void	wrbflush(void);
uint16	rdbits(int);

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
int	ebound(int, int);
char	efgetc(FILE *);
int	ipow(int, int);

extern	int		c2yuv[3][3], c2rgb[3][3];
extern	int		yq1tab[DCTW][DCTW], uvq1tab[DCTW][DCTW];

EXTERN struct {
	int *v;
	int n;
} vlitab[VLIMAX];

EXTERN	SDL_Window	*win;
EXTERN	SDL_Surface	*scr;
EXTERN	char		*errprefix;
EXTERN	char		*srcfile;
EXTERN	FILE		*fout, *fin;
