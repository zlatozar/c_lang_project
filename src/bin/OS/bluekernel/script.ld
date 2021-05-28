/* linker script */

SECTIONS
{
	. = 0x80000000;
	.text : { *(.text) }
	/*. = 0x80008000;*/
	.rodata : { *(.rodata) *(.rdata) } /*; LONG(0x12345678); LONG(LOADADDR(.rodata)) }*/
	.data /*ALIGN(0x10)*/ : { *(.data) }
	.bss : { *(.bss); LONG(0x12345678); }
	/* Placing this long value at the end of the .bss section ensures that the binary includes space for the heap, and this long value. Although somehow the long value is written at zero. The next option below using a code section (which can be easily specified at the end of the kernel_pc.asm file (or any nasm file) does ensure this long value is retained in the final binary.*/
	.code : { *(.code); LONG(0x12345678); }  /* This is to ensure the kernel binary includes space for its heap, so that user files can be safely placed after this.*/
	/DISCARD/ : { *(.eh_frame .comment .drectve) }
}
