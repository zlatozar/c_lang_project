###################################################################################################
### Master makefile for kernel project ############################################################
### (c) Paul Cuttler 2007-13 ######################################################################
###################################################################################################

###################################################################################################
### Makefile variables ############################################################################
### Tools #########################################################################################
CC		= gcc -c -m32 -fno-leading-underscore -nodefaultlibs -mno-accumulate-outgoing-args
NASM		= nasm
LD		= ld -T script.ld -nostdlib # --strip-discarded
DUMP		= objdump --disassemble-all -M intel
U		=

CREATEIMAGE	= ./tools/createimage/createimage
MERGEIMAGE	= ./tools/createimage/mergeimage

COPY		= cp -f # ignore non-existent files.
CAT		= cat
MOVE		= mv -f 
DEL		= rm -f 

### Paths #########################################################################################
SERVER_PATH	= ./server/
USER_PATH	= ./user/
DRIVER_PATH	= ./user/drivers/
LIB_PATH	= ./user/lib/
POLICY_PATH	= ./kernel/policy/
MEC_PATH	= ./kernel/mechanism/pc/
BOOT_PATH	= ./kernel/mechanism/pc/boot/

### Targets & Sources #############################################################################
OUTFILE		= kernel.img
DISK10mb	= disk10mb.img
BOOTFILE	= $(BOOT_PATH)boot.bin
KERNELFILE	= kernel.bin

INFILES		= $(SERVER_PATH)idle.bin $(SERVER_PATH)server.bin $(DRIVER_PATH)disk.bin \
		$(SERVER_PATH)excepter.bin \
		$(DRIVER_PATH)video.bin $(DRIVER_PATH)keybd.bin \
		$(USER_PATH)threads1.bin $(USER_PATH)threads2.bin \
		$(USER_PATH)user1.bin $(USER_PATH)user2.bin $(USER_PATH)user3.bin \

# arg to CREATEIMAGE to add user objects to system
CI_INFILES	= $(SERVER_PATH)idle.bin \
		$(SERVER_PATH)server.bin \
		$(DRIVER_PATH)disk.bin \
		$(SERVER_PATH)excepter.bin \
		$(DRIVER_PATH)video.bin \
		$(DRIVER_PATH)keybd.bin \
		$(USER_PATH)threads1.bin \
		$(USER_PATH)threads2.bin \
		$(USER_PATH)user1.bin \
		$(USER_PATH)user2.bin \
		$(USER_PATH)user3.bin
	
KERNEL_INIT_OBJ	= $(POLICY_PATH)kernel.o $(MEC_PATH)kernel_ia32.o

KERNEL_POLICY_OBJ   = $(POLICY_PATH)process.o $(POLICY_PATH)messages.o $(POLICY_PATH)resources.o \
		    $(POLICY_PATH)memory.o
KERNEL_MEC_OBJ	= $(MEC_PATH)process_ia32.o $(MEC_PATH)resources_pc.o $(MEC_PATH)memory_ia32.o \
		$(MEC_PATH)kernel_pc.o $(MEC_PATH)k_video.o  $(BOOT_PATH)bootdata.o

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
all: makedeps $(OUTFILE) kernel.dis
	ls -s -R | egrep "\.bin|\.bin|\.img|:" > filesizes.log
	@echo $(UNAME_S) "Machine."

makedeps:
	@echo
	@echo "***** Make kernel tools **************************"
	(cd tools/createimage; $(MAKE))
	(cd tools/processtable; $(MAKE))
	@echo
	@echo "***** Make kernel policy files *******************"
	(cd kernel/policy; $(MAKE))
	@echo
	@echo "***** Make kernel mechanism IBM-PC 386 files *****"
	(cd kernel/mechanism/pc; $(MAKE))
	@echo
	@echo "***** Make user library files ********************"
	(cd user/lib; $(MAKE))
	@echo
	@echo "***** Make server files **************************"
	(cd server/; $(MAKE))
	@echo
	@echo "***** Make driver files **************************"
	(cd user/drivers/; $(MAKE))
	@echo
	@echo "***** Make user files ****************************"
	(cd user; $(MAKE))

### Create Disassemblies ##########################################################################
kernel.dis kernel_debug.dis kernel.sym: kdump.bin
	@echo "***** Create Kernel Disassembly ******************"
	$(DUMP) kdump.bin > kernel.dis
	egrep ">:" kernel.dis | tr "\n" "\t" | tr -d '<>' | tr ":" "." > kernel_debug.dis
	egrep ">:" kernel.dis | tr -d '<>' | tr -d ":" | sed 's/^/0x/' > kernel.sym

### Create kernel/operating system image ##########################################################
$(DISK10mb):
	dd if=/dev/zero of=disk10mb.img bs=512 count=20160

$(OUTFILE): $(BOOTFILE) $(KERNELFILE) $(INFILES) $(CREATEIMAGE) $(MERGEIMAGE) $(DISK10mb)
	@echo "***** Create Kernel Image ************************"
	$(CREATEIMAGE) -v $(OUTFILE) $(BOOTFILE) $(KERNELFILE) $(CI_INFILES)
	$(MERGEIMAGE) -v $(OUTFILE) $(DISK10mb)
	cp $(OUTFILE) copy$(OUTFILE)

$(KERNELFILE): kdump.bin
	strip -O binary -s -x -o $(KERNELFILE) kdump.bin
	
### Linking #######################################################################################
kdump.bin: $(KERNEL_INIT_OBJ) $(KERNEL_POLICY_OBJ) $(KERNEL_MEC_OBJ)
	$(LD) $(KERNEL_INIT_OBJ) $(KERNEL_POLICY_OBJ) $(KERNEL_MEC_OBJ) -o kdump.bin -e $(U)kmain

### Phonies #######################################################################################
clean:
	$(DEL) *.o *.bin *.img *.inc *.dis *.log kernel.sym

cleanall:
	($(MAKE) clean)
	(cd kernel/policy; $(MAKE) clean)
	(cd kernel/mechanism/pc; $(MAKE) clean)
	(cd tools/createimage/; $(MAKE) clean)
	(cd tools/processtable/; $(MAKE) clean)
	(cd server; $(MAKE) clean)
	(cd user; $(MAKE) clean)
	(cd user/lib; $(MAKE) clean)
	(cd user/drivers; $(MAKE) clean)

# END OF FILE #
