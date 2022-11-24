CC = gcc
CF = -Wall -ggdb
PKG = sdl2 SDL2_gfx
PF = `pkg-config --cflags --libs $(PKG)` -lpng -lm
HF = src/eimg.h
OBJ = src/util.o \
      src/raw.o \
      src/sdl.o \
      src/yuv.o \
      src/dct.o \
      src/file.o \
      src/huff.o \
      src/bit.o \
      src/cmd.o

%.o:	%.c $(HF)
	$(CC) -o $@ -c $< $(CF)

eimg:	$(OBJ) $(HF)
	$(CC) $(OBJ) -o $@ $(CF) $(PF)

.PHONY:	clean

clean:
	rm -f eimg $(OBJ)
