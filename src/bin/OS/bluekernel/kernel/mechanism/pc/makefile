###################################################################################################
### Kernel Mechanisms for PC makefile for kernel project ##########################################
### (c) Paul Cuttler 2007-13 ######################################################################
###################################################################################################

###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################
CC	    = gcc -c -std=gnu99 -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args -fno-ident -O0
NASM	    = nasm
NASM_OUT    = elf32
LD	    = ld -T ../../../userscript.ld -nostdlib --strip-discarded
DUMP	    = objdump --disassemble-all -M intel

COPY	    = cp -f # ignore non-existent files.
CAT	    = cat
MOVE	    = mv -f # ignore non-existent files.
DEL	    = rm -f # ignore non-existent files.

### Paths #########################################################################################
POLICY_PATH = ../../policy/
BOOT_PATH = ./boot/

### Targets & Sources #############################################################################
POLICY_HEADERS = $(POLICY_PATH)global.h $(POLICY_PATH)kernel.h $(POLICY_PATH)types.h
MEC_HEADERS = kernel_ia32.h resources_pc.h
INC_HEADERS = kernel.inc kernel_ia32.inc resources_pc.inc
BOOTSRC = $(BOOT_PATH)boot.asm

### Platform Settings #############################################################################
ifeq ($(OS),Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),CYGWIN_NT-5.1)
	LD += -m i386pe
	U = 
        CC += -fno-stack-protector -mno-stack-arg-probe -fno-stack-check -mpush-args
        NASM_OUT = win32
    else ifeq ($(UNAME_S),CYGWIN_NT-6.1-WOW64)
	LD += -m i386pe
        CC += -fno-stack-protector -mno-stack-arg-probe -fno-stack-check -mpush-args
        U = 
        NASM_OUT = win32
    else ifeq ($(UNAME_S),CYGWIN_NT-6.1-WOW)
	LD += -m i386pe
        CC += -fno-stack-protector -mno-stack-arg-probe -fno-stack-check -mpush-args
        U = 
        NASM_OUT = win32
    else # ms-dos & djgpp
	LD += -m i386go32
	U = _
	COPY = copy
	MOVE = move
	DEL = del
	NASM_OUT = coff --prefix _
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
all: kernel_ia32.o process_ia32.o resources_pc.o memory_ia32.o k_video.o kernel_pc.o ./boot/boot.bin

### Compiling #####################################################################################
kernel_pc.o: kernel_pc.asm $(POLICY_HEADERS) $(MEC_HEADERS) $(INC_HEADERS)
	$(NASM) -f $(NASM_OUT) kernel_pc.asm -o kernel_pc.o
	
kernel.inc: $(POLICY_PATH)kernel.h
	grep ^#define $(POLICY_PATH)kernel.h | tr "#define" "%define" > kernel.inc
kernel_ia32.inc: kernel_ia32.h
	grep ^#define kernel_ia32.h | tr "#define" "%define" > kernel_ia32.inc
resources_pc.inc: resources_pc.h
	grep ^#define resources_pc.h | tr "#define" "%define" > resources_pc.inc

kernel_ia32.o: kernel_ia32.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) kernel_ia32.c
process_ia32.o: process_ia32.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) process_ia32.c
resources_pc.o: resources_pc.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) resources_pc.c
memory_ia32.o: memory_ia32.c $(POLICY_HEADERS) $(MEC_HEADERS)
	$(CC) memory_ia32.c

$(BOOT_PATH)bootdata.o: $(BOOT_PATH)bootdata.c $(BOOT_PATH)bootdata.h
	$(CC) $(BOOT_PATH)bootdata.c

k_video.o: k_video.c $(POLICY_HEADERS) $(MEC_HEADERS) $(BOOT_PATH)bootdata.h
	$(CC) -O k_video.c

$(BOOT_PATH)boot.bin: $(BOOTSRC) $(MEC_HEADERS)
	$(NASM) -f bin $< -o $@ -l $(BOOT_PATH)boot.dis
# $< is current dependency, $@ is current target.

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.img *.inc *.dis $(BOOT_PATH)*.dis $(BOOT_PATH)*.bin *.html
	
html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
