###################################################################################################
### Server Code makefile for kernel project #######################################################
### (c) Paul Cuttler 2007-13 ######################################################################
###################################################################################################

###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################

CC	    = gcc -c -std=gnu99 -fverbose-asm -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args -fno-ident -O0
NASM	    = nasm
LD	    = ld -T ../userscript.ld -nostdlib --strip-discarded #
DUMP	    = objdump --disassemble-all -M intel -h -S #-t 
U	    = 
NM	    = nm -p -g --defined-only
CODE_START  = 0
MAKESYM	    = nm -n $< | sed '/ [Aa-z] /d;s/^/0x/;s/ [A-Z]//' > $@

COPY	    = cp -f # ignore non-existent files.
CAT	    = cat
MOVE	    = mv -f # ignore non-existent files.
DEL	    = rm -f # ignore non-existent files.

### Paths #########################################################################################
LIB_PATH    = ../user/lib/
POLICY_PATH = ../kernel/policy/
MEC_PATH    = ../kernel/mechanism/pc/

### Targets & Sources #############################################################################
SERVERFILES	= idle.bin server.bin excepter.bin 
LIB_H		= $(LIB_PATH)syscall.h $(LIB_PATH)stdlib.h $(LIB_PATH)syscallext.h
LIB_OBJ		= $(LIB_PATH)syscall.o $(LIB_PATH)stdlib.o $(LIB_PATH)syscallext.o
POLICY_HEADERS	= $(POLICY_PATH)kernel.h $(POLICY_PATH)global.h $(POLICY_PATH)types.h
MEC_HEADERS	= $(MEC_PATH)resources_pc.h
KERNEL_HEADERS	= $(POLICY_HEADERS) $(MEC_HEADERS)

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
all: $(SERVERFILES) servers.dis

### Create disassemblies and flat binaries ########################################################
servers.dis: idle.dis server.dis excepter.dis server.sym
	$(CAT) idle.dis server.dis excepter.dis > servers.dis

server.bin: servertmp.o
	objcopy -O binary servertmp.o server.bin
	nm -n servertmp.o | grep " B " > serverheap.lst
server.sym: servertmp.o
	$(MAKESYM)
server.dis: servertmp.o
	$(DUMP) servertmp.o > server.dis

idle.bin: idletmp.o
	objcopy -O binary idletmp.o idle.bin
idle.dis: idletmp.o
	$(DUMP) idletmp.o > idle.dis

excepter.bin: exceptertmp.o
	objcopy -O binary exceptertmp.o excepter.bin
excepter.dis: exceptertmp.o
	$(DUMP) exceptertmp.o > excepter.dis

####################### Linking ###################################################################
idletmp.o: idle.o
	$(LD) idle.o -o idletmp.o -e $(U)Idle -Ttext $(CODE_START)
servertmp.o: server.o $(LIB_PATH)syscall.o $(LIB_PATH)stdlib.o 
	$(LD) server.o $(LIB_PATH)syscall.o $(LIB_PATH)stdlib.o -o servertmp.o -e $(U)Server -Ttext $(CODE_START)
exceptertmp.o: excepter.o $(LIB_OBJ)
	$(LD)  excepter.o $(LIB_OBJ) -o exceptertmp.o -e $(U)kmain -Ttext $(CODE_START)

### Compiling #####################################################################################
idle.o: idle.c
	$(CC) idle.c
server.o: server.c $(LIB_H) $(KERNEL_HEADERS) server.h
	$(CC) server.c
excepter.o: excepter.c $(LIB_H) $(KERNEL_HEADERS) server.h
	$(CC) excepter.c

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.img *.inc *.dis *.html

html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
