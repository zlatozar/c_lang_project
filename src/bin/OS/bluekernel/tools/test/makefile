# Makefile for test files.
# These files are used to prototype code for the kernel.

CC = g++
DEL = rm -f

all: testlib

testlib: testlib.c
	$(CC) -g -o testlib testlib.c
	objdump --disassemble-all -M intel testlib.exe > testlib.dis

clean:
	$(DEL) *.o testlib *.exe

# END OF FILE #
