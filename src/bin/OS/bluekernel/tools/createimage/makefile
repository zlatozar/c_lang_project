# Makefile for filecreate and createimage

CC = gcc -std=gnu99
DEL = rm -f

all: createimage mergeimage
	
createimage: createimage.c ../../kernel/mechanism/pc/boot/bootdata.h ../../kernel/mechanism/pc/boot/bootdata.c ../../server/server.h ../../kernel/mechanism/pc/kernel_ia32.h
	$(CC) -g -o createimage createimage.c ../../kernel/mechanism/pc/boot/bootdata.c
mergeimage: mergeimage.c
	$(CC) -o mergeimage mergeimage.c

clean:
	$(DEL) *.o createimage mergeimage *.exe

# END OF FILE #
