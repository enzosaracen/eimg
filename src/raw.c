#include "eimg.h"
#include <png.h>

Raw *png2raw(char *fname)
{
	int i, j, w, h, blen;
	char header[8];
	FILE *fp;
	png_structp pngp;
	png_infop infp;
	png_bytep *rowp;
	png_byte *row, *bp;
	Raw *raw;

	errprefix = "png2raw";
	fp = fopen(fname, "rb");
	if(fp == NULL)
		errorf("could not open %s", fname);
	fread(header, 1, 8, fp);
	if(png_sig_cmp(header, 0, 8))
		errorf("%s is not a PNG", fname);
	pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(pngp == NULL)
		errorf("png_create_read_struct failed");
	infp = png_create_info_struct(pngp);
	if(infp == NULL)
		errorf("png_create_info_struct failed");
	png_init_io(pngp, fp);
	png_set_sig_bytes(pngp, 8);
	png_read_info(pngp, infp);

	w = png_get_image_width(pngp, infp);
	h = png_get_image_height(pngp, infp);
	rowp = malloc(sizeof(png_bytep)*h);
	for(i = 0; i < h; i++)
		rowp[i] = malloc(png_get_rowbytes(pngp, infp));
	png_read_image(pngp, rowp);

	switch(png_get_color_type(pngp, infp)) {
	case PNG_COLOR_TYPE_RGB:
		blen = 3;
		break;
	case PNG_COLOR_TYPE_RGBA:
		blen = 4;
		break;
	default:
		errorf("unsupported PNG color scheme");
	}

	raw = emalloc(sizeof(Raw));
	raw->w = w;
	raw->h = h;
	raw->v = emalloc(h*sizeof(*raw->v));
	for(i = 0; i < h; i++) {
		row = rowp[i];
		raw->v[i] = emalloc(w*sizeof(**raw->v));
		for(j = 0; j < w; j++) {
			bp = &(row[j*blen]);
			raw->v[i][j][0] = bp[0];
			raw->v[i][j][1] = bp[1];
			raw->v[i][j][2] = bp[2];
		}
	}
	fclose(fp);
	errprefix = NULL;
	return raw;
}
