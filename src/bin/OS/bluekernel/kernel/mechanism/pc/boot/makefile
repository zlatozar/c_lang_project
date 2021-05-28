###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################
CC	= gcc -c -std=gnu99 -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args
NASM	= nasm
LD	= ld -T script.ld -nostdlib # --strip-discarded
DUMP	= objdump --disassemble-all -M intel
U	=

COPY	= cp -f # ignore non-existent files.
CAT	= cat
MOVE	= mv -f 
DEL	= rm -f 

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

all: brd

brd: bios_read_disk.c
	gcc -o brd bios_read_disk.c
	objdump -d brd > brd.dis

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.exe *.inc *.dis *.bin *.html
	
html:
	for i in *.c; do vim -c TOhtml -c wqa $i ; done
	for i in *.h; do vim -c TOhtml -c wqa $i ; done
	for i in *.asm; do vim -c TOhtml -c wqa $i ; done

# END OF FILE #
