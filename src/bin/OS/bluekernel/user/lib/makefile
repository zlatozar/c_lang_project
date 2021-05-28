###################################################################################################
### User Library code makefile for kernel project #################################################
### (c) Paul Cuttler 2007-13 ######################################################################
###################################################################################################

###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################
CC = gcc -c -std=gnu99 -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args -fno-ident -O0
NASM = nasm
LD = ld -T ../../userscript.ld -nostdlib
DUMP = objdump --disassemble-all -M intel
U = 
CSTUB = ../../tools/createstub/createstub
NM = nm -p -g --defined-only

COPY = cp -f # ignore non-existent files.
CAT = cat
MOVE = mv -f # ignore non-existent files.
DEL = rm -f # ignore non-existent files.

### Paths #########################################################################################
USER_PATH = ../
SERVER_PATH = ../../server/
POLICY_PATH = ../../kernel/policy/
MEC_PATH = ../../kernel/mechanism/pc/

### Targets & Sources #############################################################################
OBJECT_FILES = syscall.o stdlib.o syscallext.o
SERVER_HEADERS = $(SERVER_PATH)server.h
POLICY_HEADERS = $(POLICY_PATH)kernel.h $(POLICY_PATH)global.h $(POLICY_PATH)types.h
MEC_HEADERS = $(MEC_PATH)resources_pc.h
KERNEL_HEADERS = $(POLICY_HEADERS) $(MEC_HEADERS)

CODE_START = 0x0

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
all: $(OBJECT_FILES) libs.dis

### Create disassemblies and flat binaries ########################################################
libs.dis: syscall.dis stdlib.dis syscallext.dis

syscall.dis: syscall.o
	$(DUMP) syscall.o > syscall.dis
stdlib.dis: stdlib.o
	$(DUMP) stdlib.o > stdlib.dis
syscallext.dis: syscallext.o
	$(DUMP) syscallext.o > syscallext.dis

### Compiling #####################################################################################
syscall.o: syscall.c syscall.h $(KERNEL_HEADERS)
	$(CC) syscall.c -o syscall.o

stdlib.o: stdlib.c stdlib.h syscall.h $(KERNEL_HEADERS) $(SERVER_HEADERS)
	$(CC) stdlib.c -o stdlib.o

syscallext.o: syscallext.c syscall.h stdlib.h $(KERNEL_HEADERS) $(SERVER_HEADERS)
	$(CC) syscallext.c -o syscallext.o

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.img *.inc *.dis *.html

html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
