#include "eimg.h"

void fencode(Raw *raw)
{
	Yuv *yuv;
	Wts *w;
	
	yuv = subsamp(raw);
	w = dctyuv(yuv);
}
