###################################################################################################
### User Drivers Code makefile for kernel project #################################################
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
MAKESYM = nm -n $< | sed '/ [Aa-z] /d;s/^/0x/;s/ [A-Z]//' > $@

CODE_START = 0x0

COPY = cp -f # ignore non-existent files.
CAT = cat
MOVE = mv -f # ignore non-existent files.
DEL = rm -f # ignore non-existent files.

### Paths #########################################################################################
LIB_PATH = ./../lib/
SERVER_PATH = ../../server/

### Targets & Sources #############################################################################
DRIVERFILES = video.bin keybd.bin disk.bin
LIB_H = $(LIB_PATH)syscall.h $(LIB_PATH)syscallext.h $(LIB_PATH)stdlib.h
LIB_OBJ = $(LIB_PATH)syscall.o $(LIB_PATH)syscallext.o $(LIB_PATH)stdlib.o
SERVER_H = $(SERVER_PATH)server.h

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
all: $(DRIVERFILES) drivers.dis

### Create disassemblies and flat binaries ########################################################
drivers.dis: video.dis video.sym keybd.dis keybd.sym disk.dis disk.sym
	$(CAT) video.dis keybd.dis disk.dis > drivers.dis

disk.bin: disktmp.o
	objcopy -O binary disktmp.o disk.bin
disk.dis: disktmp.o
	$(DUMP) disktmp.o > disk.dis
disk.sym: disktmp.o
	$(MAKESYM)

keybd.bin: keybdtmp.o
	objcopy -O binary keybdtmp.o keybd.bin
keybd.dis: keybdtmp.o
	$(DUMP) keybdtmp.o > keybd.dis
keybd.sym: keybdtmp.o
	$(MAKESYM)

video.bin: videotmp.o
	objcopy -O binary videotmp.o video.bin
video.dis: videotmp.o
	$(DUMP) videotmp.o > video.dis
video.sym: videotmp.o
	$(MAKESYM)

### Linking #######################################################################################
videotmp.o: video.o $(LIB_OBJ)
	$(LD) video.o $(LIB_OBJ) -o videotmp.o -e $(U)kmain -Ttext $(CODE_START)
keybdtmp.o: keybd.o $(LIB_OBJ)
	$(LD) keybd.o $(LIB_OBJ) -o keybdtmp.o -e $(U)kmain -Ttext $(CODE_START)

disktmp.o: disk.o $(LIB_OBJ)
	$(LD) disk.o $(LIB_OBJ) -o disktmp.o -e $(U)kmain -Ttext $(CODE_START)

### Compiling #####################################################################################
video.o: video.c $(LIB_H) $(SERVER_H)
	$(CC) video.c
keybd.o: keybd.c $(LIB_H) keybd.h $(SERVER_H)
	$(CC) keybd.c
disk.o: disk.c disk.h $(LIB_H) $(SERVER_H)
	$(CC) disk.c

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.img *.inc *.dis *.html

html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
