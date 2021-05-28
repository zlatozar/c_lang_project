###################################################################################################
### User Code makefile for kernel project #########################################################
### (c) Paul Cuttler 2007-13 ######################################################################
###################################################################################################

###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################

CC	= gcc -c -std=gnu99 -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args -fno-ident -O0
NASM	= nasm
LD	= ld -T ../userscript.ld -nostdlib
DUMP	= objdump --disassemble-all -M intel

 # underscore?
U	= 
NM	= nm -p -g --defined-only

COPY	= cp -f # ignore non-existent files.
CAT	= cat
MOVE	= mv -f # ignore non-existent files.
DEL	= rm -f # ignore non-existent files.

### Paths #########################################################################################
LIB_PATH    = ./lib/
SERVER_PATH = ../server/

### Targets & Sources #############################################################################
USERFILES	= threads1.bin threads2.bin user1.bin user2.bin user3.bin
LIB_H		= $(LIB_PATH)syscall.h $(LIB_PATH)stdlib.h $(LIB_PATH)syscallext.h
LIB_OBJ		= $(LIB_PATH)syscall.o $(LIB_PATH)stdlib.o $(LIB_PATH)syscallext.o
SERVER_HEADERS	= $(SERVER_PATH)server.h

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
all: $(USERFILES) users.dis

users.dis: threads1.dis threads2.dis user1.dis user2.dis user3.dis
	$(CAT) threads1.dis threads2.dis user1.dis user2.dis user3.dis > users.dis

threads1.dis: threads1tmp.o
	$(DUMP) threads1tmp.o > threads1.dis
threads2.dis: threads2tmp.o
	$(DUMP) threads2tmp.o > threads2.dis
user1.dis: user1tmp.o
	$(DUMP) user1tmp.o > user1.dis
user2.dis: user2tmp.o
	$(DUMP) user2tmp.o > user2.dis
user3.dis: user3tmp.o
	$(DUMP) user3tmp.o > user3.dis

### Create disassemblies and flat binaries ########################################################
threads1.bin: threads1tmp.o
	objcopy -O binary threads1tmp.o threads1.bin
threads2.bin: threads2tmp.o
	objcopy -O binary threads2tmp.o threads2.bin
user1.bin: user1tmp.o
	objcopy -O binary user1tmp.o user1.bin
user2.bin: user2tmp.o
	objcopy -O binary user2tmp.o user2.bin
user3.bin: user3tmp.o
	objcopy -O binary user3tmp.o user3.bin

### Linking #######################################################################################
threads1tmp.o: threads1.o $(LIB_OBJ)
	$(LD) threads1.o $(LIB_OBJ) -o threads1tmp.o -e $(U)kmain
threads2tmp.o: threads2.o $(LIB_OBJ)
	$(LD) threads2.o $(LIB_OBJ) -o threads2tmp.o -e $(U)kmain
user1tmp.o: user1.o $(LIB_OBJ)
	$(LD) user1.o $(LIB_OBJ) -o user1tmp.o -e $(U)kmain
user2tmp.o: user2.o $(LIB_OBJ)
	$(LD) user2.o $(LIB_OBJ) -o user2tmp.o -e $(U)kmain
user3tmp.o: user3.o $(LIB_OBJ)
	$(LD) user3.o $(LIB_OBJ) -o user3tmp.o -e $(U)kmain

### Compiling #####################################################################################
threads1.o: threads1.c $(LIB_H) $(SERVER_HEADERS)
	$(CC) threads1.c
threads2.o: threads2.c $(LIB_H) $(SERVER_HEADERS)
	$(CC) threads2.c
user1.o: user1.c $(LIB_H) $(SERVER_HEADERS)
	$(CC) user1.c
user2.o: user2.c $(LIB_H) $(SERVER_HEADERS)
	$(CC) user2.c
user3.o: user3.c $(LIB_H) $(SERVER_HEADERS)
	$(CC) user3.c

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.dis *.html

html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
