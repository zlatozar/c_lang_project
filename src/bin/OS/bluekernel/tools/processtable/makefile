# Makefile for filecreate and createimage

CC = gcc -g
DEL = rm -f

ptd: processtable.c ../../kernel/policy/global.h
	$(CC) -o ptd processtable.c
	
install:
	sudo cp ptd /usr/bin

clean:
	$(DEL) *.o ptd *.exe

# END OF FILE #
