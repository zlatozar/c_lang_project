; -------------------------------------------------------------------------------------------------
; IA-32 boot code
;
;	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
;	International License. To view a copy of this license, visit
;	http://creativecommons.org/licenses/by-nc-nd/4.0/
;	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
;
;	Also see http://www.bluekernel.com.au
;
;	copyright Paul Cuttler 2017
; -------------------------------------------------------------------------------------------------

;; Documentation annotations:
;; @A web_mec_pc_intro_boot Kernel Mechanisms Boot Code
;; @A+

%include "kernel_ia32.inc" ; NB: the path here is relative to where nasm was run, which 
	; is in mechanism/pc directory, not mechanism/pc/boot directory.

[BITS 16]	; the bios starts out in 16-bit real mode
[ORG 7c00h]
[CPU 586]	;assume 586 from the start, although more thorough boot code would
			;test for the cpu version before assuming anything.

%define REAL_STACK_BASE		0x2000
%define REAL_STACK_PTR		0xFFFC	; align the stack to 32-bit words.
%define NASM_MAGIC_BREAK	xchg bx, bx
; -------------------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------------------
; BOOT SECTOR
; -------------------------------------------------------------------------------------------------

jmp 0:BootSectorStart	; skip over our data and functions, ensure CS=0, start is relative to 7c00h

; -------------------------------------------------------------------------------------------------
; Data (starts at offset 5. First comment after each initial value shows data offset.)
; -------------------------------------------------------------------------------------------------
	sectors_required	dw 0x00A0	;0x05; may transfer more than this, up to a whole track more.
	sec_transferred		dw 0x0000	;0x07;

	boot_drive			db 0		;0x09; 0 = floppy A, 0x80 = first hard disk
	drive_type			db 0		;0x0A; bl = drive type (04 for 1.44M)		04
	disk_retries		db 4		;0x0B; num tries + 1
	num_drives			db 0		;0x0C; dl = num drives					01

	;GetDiskGeometry saves next 3 max values
	max_sec				db 0		;0x0D; cl:5-0 = max sector number			12h (18)
	max_head			db 0		;0x0E; dh = max head num					01
	max_cyl				db 0		;0x0F; cl:7-6 + ch = max cylinder number	004f (79)

	curr_sec			db 2		;0x10;(starts at sector 1 not 0, start at 2 because boot is sec.1)(1-63)
	curr_head			db 0		;0x11;
	curr_cyl			db 0		;0x12;
	seg_start			dw 0x07e0	;0x13;
	seg_end  			dw 0x07e0	;0x15;

	num_sec				db 1		;0x17; number of sectors to load. Init to 1 because boot sector is already loaded.

	reset_halt_msg		db 'rht',13,10,0 ;0x18; 0x72 0x68 0x74 0x0d 0x0a 0x00
	halt_msg			db 'ht',13,10,0  ;0x1E; 0x68 0x74 0x0d 0x0a 0x00 
	disk_error_msg		db 'de',0        ;0x23; 0x64 0x65 0x00
	disk_status			dw 0x0000        ;0x26;
	
; -------------------------------------------------------------------------------------------------
; The main code of our boot loading process
; -------------------------------------------------------------------------------------------------
BootSectorStart:
    mov ax, cs
    mov ds, ax
    mov es, ax
	mov [boot_drive], dl ; save the drive we booted from
	cli	; clear interrupts while we initialise system.
		; NmiExc needs to be controlled through I/O port.
	; Setup Stack
	mov ax,REAL_STACK_BASE   ; NB: need to do shr 4.
	mov ss,ax
	mov sp,REAL_STACK_PTR   ;
	call GetDiskGeometry
	call LoadSectors
	call SetVideoMode
	call SetA20
	jmp Loader

; -------------------------------------------------------------------------------------------------
; Functions used in the boot-loading process
; -------------------------------------------------------------------------------------------------
SetVideoMode:
		mov ah, 00h ; set video mode, with INT 10h.
		mov al, 03h ; desired video mode.  03h is std text mode 80x25.
			;; 13h works well so far for graphics (320x200 linear 256-color mode),
			;; 12h should be best (640x480 planar 16-bit color mode).
		mov bx, 30h
		int 10h
		;;INT 0x10, AX=0x4F02, BX=mode
		;;Set Video Mode. Call this with the mode number you decide to use. 
; 		mov ah, 02h ; set cursor position
; 		mov dh, 0  ; row
; 		mov dl, 0  ; column
; 		mov bh, 0  ; page
; 		int 10h
		mov ah, 01h ;set text-mode cursor shape - turn it off.
		mov ch, 20h
		mov cl, 0
		int 10h
		ret
; -------------------------------------------------------------------------------------------------
Message:		  ; Dump ds:si to screen.
		mov ah, 0eh	; put character
		mov bl, 8Fh	; attribute, BH=page, BL=foregnd color
		mov bh, 0
	.message_loop:
		lodsb		; load byte at ds:si into al
		or al, al	  ; test if character is 0 (end)
		jz .done
		int 0x10	  ; call BIOS
		jmp .message_loop
	.done:
		ret
;; @A-
; -------------------------------------------------------------------------------------------------
;message_num:	   ; Dump hex numbers to screen from ds:si pointer.
		; need to specify num of bytes in cx.
; 	.num_loop:
; 		lodsb		; load-string-byte: load byte at ds:si into al
; 		mov dl, al
; 		and dl, 0Fh ; put low nibble in dl
; 		and al, 0F0h  ; high nibble - print high nibble first.
; 		shr al, 4
; 		cmp al, 0Ah
; 		jge .ascii_hex
; 		add al, 30h	; offset ASCII, 30h='0'
; 		jmp .hex_next
; 	.ascii_hex:
; 		add al, 37h	; offset ASCII, 0Ah+37h=41h='A'
; 	.hex_next:
; 					; al = char to put.
; 		mov ah, 0eh	; put character, ah=function
; 		mov bl, 07h	; attribute, BH=page, BL=foregnd color
; 		mov bh, 0
; 		int 0x10	  ; call BIOS
; 		mov al, dl		; mov high nibble to al
; 		cmp al, 0Ah	; go through same stuff as above.
; 		jge .ascii_hex2
; 		add al, 30h
; 		jmp .hex_next2
; 	.ascii_hex2:
; 		add al, 37h
; 	.hex_next2:
; 		int 0x10
; 		dec cx
; 		jz .done_num
; 		jmp .num_loop
; 	.done_num:
; 		mov al, 13
; 		int 0x10
; 		mov al, 10
; 		int 0x10 ; this does CR-LF.
		;ret
; -------------------------------------------------------------------------------------------------
GetKey:
; 		mov ah, 0		 ; wait for key
; 		int 016h
; 		ret
; -------------------------------------------------------------------------------------------------
;; @A+
Reboot:
		mov si, end_msg
		call Message
		call GetKey	    ; wait for a key
		jmp 0FFFFh:0000	; jump to FFFF:0000 (FFFF0)(BIOS Reboot)
; -------------------------------------------------------------------------------------------------
SetA20:
		;cli
		xor cx, cx
	.clear_buffer:
		in al, 64h		; get input from keyboard status port
		test al, 02h	; test the buffer full flag
		loopnz .clear_buffer ; loop until buffer is empty
		mov al, 0D1h	; keyboard: write to output port
		out 64h, al		; output command to keyboard
	.clear_buffer2:
		in al, 64h		; wait until the buffer is empty again
		test al, 02h	; test the buffer full flag
		loopnz .clear_buffer2
		mov al, 0dfh	; keyboard: set A20
		out 60h, al		; send it to the keyboard controller
		mov cx, 14h
	.wait_kbc:			; this is approx. a 25uS delay to wait
		out 0edh, ax	; for the kb controller to execute our
		loop .wait_kbc	; command.
		; the A20 line is on now.
		ret
; -------------------------------------------------------------------------------------------------
GetDiskGeometry:
		mov ax,0
		mov es, ax
		mov di, 0
		mov ah, 08h
		mov dl,[boot_drive]
		int 13h ; get disk geometry in following registers and save to memory

		; bl = drive type (04 for 1.44M)
		mov [drive_type], bl

		; cl:5-0 = max sector number
		mov bl, cl
		and bl, 3Fh
		mov [max_sec], bl

		; cl:7-6 + ch = max cylinder number
		mov bx, 0
		mov bh, cl
		shr bh, 6
		mov bl, ch
		mov [max_cyl], bl

		; dh = max head num
		mov [max_head], dh

		; dl = num drives
		mov [num_drives], dl

		; Do the following to print out disk geometry
		;mov si, max_cyl ;max_cyl (4f00),max_sec (12),max_head (01)
		;mov cx, 4
		;call message_num

		; es:di = drive parameter table (floppy only)
		ret
; -------------------------------------------------------------------------------------------------
CalcSegEnd:
	; seg_end = seg_start + num_sec*512>>4
	; seg_start initialised to 0x07e0 (ie 0x7e00 >> 4)
		mov ax, 32 ; sector size>>4 (512>>4=32) due to seg reg shift 4
		mov bx, 0 ; need to clear because num_sec is only byte width.
		mov bl, [num_sec]
		mul bx  ; ax*bx, result in dx:ax = num_sec*512>>4, but should be small & just in ax.
		add ax, [seg_start]  ;seg_end = seg_start + num_sec*512>>4
		mov [seg_end], ax
		ret
; -------------------------------------------------------------------------------------------------
DiskReset:
		; first, reset the disk controller
		mov ah, 0	; reset function
		mov dl, [boot_drive]
		int 13h
		jc Reboot ; condition code indicates error, Reboot on error
		dec byte [disk_retries]
		jnz .new_try
		mov si, reset_halt_msg
		call Message
		cli
		hlt
	.new_try:
		mov [disk_retries], al
		ret
; -------------------------------------------------------------------------------------------------
BiosRead:
		call DiskReset

		mov ax, [seg_start]
		mov es,ax
		mov bx,0	; sectors get loaded starting at es:bx
		mov ah,2	; function number - read sector
		mov al,[num_sec]
		mov ch,[curr_cyl]
		mov cl,[curr_sec]
		mov dh,[curr_head]
		mov dl,[boot_drive]
		int 13h	; read disk
		jnc .finish ; success.
		mov [disk_status], ax ; disk error status.
		mov si, disk_error_msg    ;386 gives 2320, laptop 0004
		call Message
		;mov si, disk_status
		;mov cx, 2
		;call message_num
		jmp BiosRead ; if there's an error then we'll try again.
	.finish:
		mov byte [disk_retries], 4
		mov al, [num_sec]
		add [sec_transferred], al
		ret
; -------------------------------------------------------------------------------------------------
LoadSectors:
		cmp word [sectors_required], 0
		jle .LoadSectorsEnd ; while (sectors_required > 0)
		;{
			; if (curr_sec > max_sec)
			mov al, [curr_sec]
			cmp al, [max_sec]
			jle .L0
			;{
				inc byte [curr_head] ; curr_head++
				; if (curr_head > max_head)
				mov al, [curr_head]
				cmp al, [max_head] ; al - max_head.
				jle .L2	;{
					mov byte [curr_head], 0 ; curr_head = 0
					inc byte [curr_cyl] ; curr_cyl++ }
				.L2:
					mov al, [max_sec]
					mov [num_sec], al ; num_sec = max_sec
					mov byte [curr_sec], 1 ; curr_sec = 1
				jmp .L1
			; else
			.L0:
				mov al, [max_sec] ; num_sec = max_sec - num_sec
				sub al, byte [num_sec]
				mov [num_sec], al
		.L1: ; after if.
			; seg_end = seg_start + num_sec * SECTOR_SIZE;
			call CalcSegEnd
			; check DMA boundary
			mov ax, [seg_start]
			shr ax, 12 ;seg_start >> 16
			mov bx, [seg_end]
			shr bx, 12 ;seg_end >> 16
			cmp ax, bx
			je .L4
			; if not equal, ie in different 64kb region
				and word [seg_end], 0xF000
				mov bx, [seg_end]
				sub bx, [seg_start] ; seg_end - seg_start
				shr bx, 5 			; " " / SECTOR_SIZE
				mov [num_sec], bl
		.L4: 
			mov ax, 0
			mov al, [num_sec]
			sub [sectors_required], ax ; sectors_required -= num_sec;
			call BiosRead
			mov bl, [num_sec]
			add [curr_sec], bl ; curr_sec += num_sec;
			mov ax, [seg_end]
			mov [seg_start], ax
			jmp LoadSectors
	.LoadSectorsEnd:
		ret

	; The boot sector is supposed to have 0xAA55 at the end of
	; the sector (the word at 510 bytes) to be loaded by the BIOS...
	times 510-($-$$) db 0 ; fill in-between memory with 0.
	dw 0xAA55 ; boot signature.

; -------------------------------------------------------------------------------------------------
; Loader
; -------------------------------------------------------------------------------------------------
Loader: ; this code starts at 7e00
	cli
	call CopyGDT
	lgdt [GDT_reg]
	call CopyIDT ; is IDT required yet? We can leave this to proper kernel code to init.
	lidt [IDT_reg]
	call BuildPages;
	;----- Move to Protected Mode ---
 	mov eax, PDk 
 	mov cr3, eax
 	mov esp, BOOT_STACK ; 80103FF0h ; top of stack/Data Seg, at phys 0x3ff0
	mov eax, cr0 ; load the control register
 	or eax, 80000007h ; set bit 0 for protected mode, bit 31 for paging.
	; need to set NE (bit 5, ie +32) and MP (bit 1, +2) in cr0
	; or, EM set (bit 2 ie +4), MP cleared, NE set. try +6
	mov cr0, eax ; copy it back to the control register
 	mov eax, PDk ;reload CR3 to clear all TLB entries.
 	mov cr3, eax
	jmp CSk_SEL:ProtectedModeJump	; Final jump to protected mode code.

; -------------------------------------------------------------------------------------------------
	loader_msg		db 'Loader running',13,10,0
	copy_IDT_msg	db 'creating IDT',13,10,0
	copy_GDT_msg	db 'creating GDT',13,10,0
; -------------------------------------------------------------------------------------------------
; Functions used in the Loader process
; -------------------------------------------------------------------------------------------------
CopyIDT:
; 		mov si, copy_IDT_msg
; 		call Message
		cld
		; ebx contains counter for number of IDT entries, starting
		; at 0 and through to 31.
		mov bx, [pIDT] ; start of IDT segment.
		shr bx, 4	; shift right to compensate for segment shift left.
		mov es, bx
		mov ax, 0		; ax contains IDT entry num.  It is incremented by 8 bytes.
	.entriesloop:
		mov si, OffsetL ;start of predefined interrupt gate.
		mov di, ax	; entry number * 8.
		mov cx, 8/4 ; number of bytes/dword size.
		rep movsd
		add ax, 8		; next entry is 8 bytes away.
		cmp ax, 64*8	; when 64 entries have been created (for exceptions), then finish.
		jl  .entriesloop ; jump if ax < 64*8.
		ret
; -------------------------------------------------------------------------------------------------
CopyGDT: ; move GDT entries to pGDT
; 		mov si, copy_GDT_msg
; 		call Message
		; clear direction for auto-increment
		cld
		;ds:si=source string
		mov si, LimitL0 ;start location of array to copy.
		;es:di=destination string
		mov ax, GDT_BASE_PHYS ;0x200 ;[pGDT]
		shr ax, 4 ; to set base to 0020h, ie to counteract 20-bit real mode shl 4.
		mov es, ax
		;dest. offset.
		mov di,0
		mov cx,5*8/4 ;num of dwords: num of descriptors * desc length / size of dword.
			; this is for the initial, pre-defined descriptors:
			; NULL, Kernel CS/DS, User CS/DS.
		rep movsd
		ret
; -------------------------------------------------------------------------------------------------
VideoOut:
		mov edi, 10000h ;0B8000h
 		mov cx,10
		mov ah, 7
		mov al, 'a'
		a32	stosw
		ret
; -------------------------------------------------------------------------------------------------
ProtectedModeReboot:
		mov si, pm_mode_reboot_msg
		mov dx,3
		call VideoOut
		mov dx,4
		mov si, end_msg
		call VideoOut
		mov ecx,0FFFFh
	.long_loop:
		cmp ecx,0
		jz .bios_reboot
		dec ecx
		jmp .long_loop
	.bios_reboot:
		jmp 0FFFF0h ;0FFFFh:0000	; jump to FFFF:0000 (FFFF0)(BIOS Reboot)
; -------------------------------------------------------------------------------------------------
BuildPages:
		; Fill PD with PT entries, bx is location, ax is entry.
		mov eax, (PT0k + IA32_KERNEL_RW)
		mov [PDk_0], eax
		mov eax, (PT1k + IA32_KERNEL_RW)
		;mov [(PDk + ((0x80000000 >> 22)*4))], eax
		mov [PDk_1], eax
	;(PDk + ((0x80000000 >> 22)*4))
		; Fill PT0 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		mov cx, 9 ; Must cover phys addresses 0 - 0x8fff.
		mov edx, (0 << PAGE_ADDRESS_SHIFT) + IA32_KERNEL_RW
		mov eax, PT0k_0
	.page_loop0:
		mov [eax], edx
		add edx, PAGE_SIZE ; increment by one page.
		add eax, 4 ; increment by one page table entry address.
		dec cx
		jnz .page_loop0
		
		; Fill PT1 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		; Fill PT1 with kernel stack page entry:
		mov eax, (BOOT_STACK_PHYS + IA32_KERNEL_RW)
		mov [PT1k_STACK], eax
		; All this to divide number of kernel blocks/PAGE_SIZE/boot_block_size 
		; DIV r/m16 Unsigned divide DX:AX by r/m16, with result
		; stored in AX← Quotient, DX ←Remainder
		xor dx, dx; 
		mov ax, (PAGE_SIZE)
		mov ebx, [BOOT_BLOCK_SIZE_16] ;
		div bx; ax = dx:ax/bx with remainder in dx.
		xchg bx, ax
		mov eax, [(BOOT_TABLE_KERNEL_SIZE_16)] ; kernel size in blocks
		xor dx, dx
		div bx ; ax should be num pages to set.
		mov cx, ax
		inc cx ; in case there was a remainder from 2nd DIV.
		mov edx, (KERNEL_BASE_PHYS + IA32_KERNEL_RW)
		mov eax, PT1k_0 ; address to store value.
	.page_loop:
		mov [eax], edx
		add edx, PAGE_SIZE ; increment by one page.
		add eax, 4 ; increment by one page table entry address.
		dec cx
		jnz .page_loop
		ret
; -------------------------------------------------------------------------------------------------
	times 1024-($-$$) db 0 ; fill in end-memory with 0.
; -------------------------------------------------------------------------------------------------

; -------------------------------------------------------------------------------------------------
; System Tables
; -------------------------------------------------------------------------------------------------
; IDT
	; the next 6 bytes are used to load IDTR
	IDT_reg		dw	IDT_LIMIT;
	pIDT		dd	IDT_BASE_PHYS ; 0MB, location of IDT
	; First 32 entries are for exceptions.  They will all
	; use the same gate to begin with.
	OffsetL		dw	8500h ;isr32+7C00h ; offset0-15 from segment base
	Selector	dw	CSk_SEL ;0008h ; this is CSk selector.
	WordCount	db	0000h ; 5-bit value, not used.
	AccessInt	db	PRESENT_BIT + DPL0 + INT_GATE_DESC ;P-bit (80h), DPL=0,S=0; Type=0E(int gate)
	OffsetH		dw	0000h ; offset16-31 from seg base.
; -------------------------------------------------------------------------------------------------
; GDT
	; the next 6 bytes are used to load GDTR, and are needed for a register indirect load of GDT.
	GDT_reg		dw	GDT_LIMIT		; 01FFh ; limit/size-1, 64 entries.
	pGDT		dd	GDT_BASE_PHYS	; 0200h ; base address

	; Entry 0 = null selector
	LimitL0		dw	0000h	;0..15
	BaseL0		dw	0000h	;0..15
	BaseM0		db	00h	;16..23
	Access0		db	00h	;P|DPL0-1|S|Type0-2|A
	G_LimitH0	db	00h	;G|D|0|AVL|Limit16..19
	BaseH0		db	00h	;24..31

	; Entry 1 = Kernel CS
	LimitL1		dw	LIMIT_LOW					 ;FFFFh
	BaseL1		dw	BASE_LOW					 ;0000h
	BaseM1		db	BASE_MID					 ;00h
	Access1		db	PRESENT_BIT + DPL0 + CS_DESC ;98h
	G_LimitH1	db	PAGE_GRAN_32BIT	+ LIMIT_HIGH ;CFh
	BaseH1		db	BASE_HIGH					 ;00h

	; Entry 2 = Kernel DS
	LimitL2		dw	LIMIT_LOW
	BaseL2		dw	BASE_LOW
	BaseM2		db	BASE_MID
	Access2		db	PRESENT_BIT + DPL0 + DS_DESC ;92h
	G_LimitH2	db	PAGE_GRAN_32BIT	+ LIMIT_HIGH
	BaseH2		db	BASE_HIGH

	; Entry 4 = User CS
	LimitL4		dw	LIMIT_LOW
	BaseL4		dw	BASE_LOW
	BaseM4		db	BASE_MID
	Access4		db	PRESENT_BIT + DPL3 + CS_DESC ;F8h
	G_LimitH4	db	PAGE_GRAN_32BIT	+ LIMIT_HIGH
	BaseH4		db	BASE_HIGH

	; Entry 5 = User DS
	LimitL5		dw	LIMIT_LOW
	BaseL5		dw	BASE_LOW
	BaseM5		db	BASE_MID
	Access5		db	PRESENT_BIT + DPL3 + DS_DESC ;F2h
	G_LimitH5	db	PAGE_GRAN_32BIT	+ LIMIT_HIGH
	BaseH5		db	BASE_HIGH
; end GDT -----------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------------------
	pm_mode_reboot_msg db 'protected mode reboot',13,10,0;
	times 2048-5-($-$$) db 0 ; fill in end-memory with 0.
	end_msg db 'PC',13,10,0 ;

; -------------------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------------------
; PROTECTED MODE, 32-bits
; -------------------------------------------------------------------------------------------------
[BITS 32]
; This is at 0x8400 (2048), or offset 0x0800
ProtectedModeJump:
	mov ax, DSk_SEL
	mov ds, ax
	mov es, ax
  	mov ss, ax
  	mov ax, 0
  	mov fs, ax ; this effectively disables fs and gs from use in the kernel.
  	mov gs, ax
	jmp KERNEL_BASE ;80000000h, defined in kernel_ia32.h
	; This jumps to 0x00009000, which should be physical memory location
	; for C code for the kernel. The basic paged protected mode is set up.
	; - Can use "times" to set function for basic interrupt gate here:
	times 4096-($-$$) db 0Ah;
; -------------------------------------------------------------------------------------------------
;; @A-
; END OF FILE ;
