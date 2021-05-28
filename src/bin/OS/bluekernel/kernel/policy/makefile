###################################################################################################
### Kernel policy makefile for kernel project #####################################################
### (c) Paul Cuttler 2007-13 ######################################################################
###################################################################################################

###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################
CC	= gcc -c -std=gnu99 -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args -fno-ident -O0
NASM	= nasm
LD	= ld -T ../../userscript.ld -nostdlib --strip-discarded
DUMP	= objdump --disassemble-all -M intel

COPY	= cp -f # ignore non-existent files.
CAT	= cat
MOVE	= mv -f # ignore non-existent files.
DEL	= rm -f # ignore non-existent files.

### Paths #########################################################################################
MEC_PATH	= ../mechanism/pc/

### Targets & Sources #############################################################################
POLICY_HEADERS	= global.h kernel.h types.h mec.h
MEC_HEADERS	= $(MEC_PATH)resources_pc.h $(MEC_PATH)kernel_ia32.h

### Platform Settings #############################################################################
ifeq ($(OS),Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),CYGWIN_NT-5.1)
	LD += -m i386pe
	U = 
        CC += -fno-stack-protector -mno-stack-arg-probe -fno-stack-check -mpush-args
    else ifeq ($(UNAME_S),CYGWIN_NT-6.1-WOW64)
	LD += -m i386pe
        CC += -fno-stack-protector -mno-stack-arg-probe -fno-stack-check -mpush-args
        U = 
    else ifeq ($(UNAME_S),CYGWIN_NT-6.1-WOW)
	LD += -m i386pe
        CC += -fno-stack-protector -mno-stack-arg-probe -fno-stack-check -mpush-args
        U = 
    else # ms-dos & djgpp
	LD += -m i386go32
	U = _
	COPY = copy
	MOVE = move
	DEL = del
	endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LD += -m elf_i386
        CC += -fno-stack-protector
    endif
endif

###################################################################################################
### Makefile start ################################################################################
all: kernel.o process.o messages.o memory.o resources.o

### Compiling #####################################################################################
kernel.o: kernel.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) kernel.c
process.o: process.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) process.c
messages.o: messages.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) messages.c
resources.o: resources.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) resources.c
memory.o: memory.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) memory.c

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.img *.inc *.dis *.html
	
html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
