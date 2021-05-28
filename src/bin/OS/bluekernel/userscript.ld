/* linker script 
 This is used by /user, /user/lib */

SECTIONS
{
	.text : { *(.text) }
	.rodata : { *(.rdata) }
	.rdata  : { *(.rdata)  } /* depends upon compiler environment whether it creates .rodata or .rdata sections, but they are essentially the same */
	.data : { *(.data) }
	.bss /*ALIGN(0x1000)*/ : { *(.bss) }
	/DISCARD/ : { *(.eh_frame .comment .drectve ) }
}
