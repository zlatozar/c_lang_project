///////////////////////////////////////////////////////////////////////////////////////////////////
/* Kernel Mechanisms Initialisation
	kernel_ia32.h

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __KERNEL_IA32_H
#define __KERNEL_IA32_H

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Kernel Mechanism Definitions ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG asm("nop")
#define BREAK asm("xchg %bx, %bx")
/* the BREAK instruction is used by Bochs emulator debugger to break execution and return to
 * debugger command line. The following line in Bochs configuration file has to be enabled:
 * magic_break: enabled=1 */ 

#define CPU_PORTS			1
#define PROCESS_MECHANISMS	28

// Privilege Levels ///////////////////////////////////////////////////////////////////////////////
#define PL0			0
#define PL1			1
#define PL2			2
#define PL3			3
#define DPL0		0x00
#define DPL1		0x20
#define DPL2		0x40
#define DPL3		0x60

// Selectors //////////////////////////////////////////////////////////////////////////////////////
#define NULL_SEL	((0 * 8) + PL0)
#define CSk_SEL		((1 * 8) + PL0)
#define DSk_SEL		((2 * 8) + PL0)
#define CSu_SEL 	((3 * 8) + PL3)
#define DSu_SEL 	((4 * 8) + PL3)

// Segment Descriptors ////////////////////////////////////////////////////////////////////////////
#define SEGMENT_LIMIT		0xFFFFF
		// maximum for 20-bit value.

#define SEGMENT_BASE		0

#define LIMIT_LOW_16		0xFFFF
#define LIMIT_HIGH_4		0xF0000
#define LIMIT_HIGH_SHIFT	16
#define BASE_LOW_16			0
#define BASE_MID_8			0
#define BASE_MID_SHIFT		16
#define BASE_HIGH_8			0
#define BASE_HIGH_SHIFT		24

#define LIMIT_LOW			(SEGMENT_LIMIT & LIMIT_LOW_16)
#define LIMIT_HIGH			((SEGMENT_LIMIT & LIMIT_HIGH_4) >> LIMIT_HIGH_SHIFT)
#define BASE_LOW			(SEGMENT_BASE & BASE_LOW_16)
#define BASE_MID			(SEGMENT_BASE & BASE_MID_8)
#define BASE_HIGH			(SEGMENT_BASE & BASE_HIGH_8)
#define PAGE_GRAN_32BIT		(0x80 + 0x40)
//gran.=1 (page), d=1 (32-bit), other bits are AVL?

#define CS_DESC				0x18
//this sets S=1, E=1, C=0, R=0, A=0.  P=0 and DPL=0, but are set elsewhere.

#define DS_DESC				0x12
//this sets S=1, E=0, ED=0 (expand up), W=1, A=0.  P, DPL as above.

#define PRESENT_BIT			0x80

/*The following descriptors are system descriptors, so S=0.*/
#define TSS_DESC			0x09
#define LDT_DESC			0x02
#define TASK_GATE_DESC		0x05
#define CALL_GATE_DESC		0x0C
#define INT_GATE_DESC		0x0E
#define TRAP_GATE_DESC		0x0F

// Task State Segment Sizes ///////////////////////////////////////////////////////////////////////
#define TSS_SIZE			0x68
//limit = size-1

#define TSS_BITMAP_SIZE		(0x100-TSS_SIZE)
#define TSS_BITMAP_BASE		(TSS_SIZE-1)
//0xDFFF //DFFF is maximum, and if > TSS limit then no bitmap.

// Descriptor Tables //////////////////////////////////////////////////////////////////////////////
#define IDT_BASE_PHYS		0x0000
#define IDT_BASE			(KERNEL_LOWMEM + IDT_BASE_PHYS)
//0x80100000 

#define IDT_SIZE			(64*8)
//0x200

#define IDT_LIMIT			(IDT_SIZE-1)
// 01FFh ; 64 entries * 8 bytes = 200h-1.

#define NUM_IDT_ENTRIES		(IDT_SIZE/8)

#define GDT_BASE_PHYS		(IDT_BASE_PHYS+IDT_SIZE)
#define GDT_BASE			(KERNEL_LOWMEM + GDT_BASE_PHYS)
//0x80100200 

#define GDT_BASE_1			(GDT_BASE + 8)
// GDT base for entry #1 (ie after entry 0).

#define GDT_SIZE			IDT_SIZE
#define GDT_LIMIT			IDT_LIMIT
#define NUM_GDT_ENTRIES		NUM_IDT_ENTRIES

//LDTs are not used.
//#define LDT_BASE			0x80100400
//#define LDT_SIZE			0x1
#define LDT_SEL				0

#define IDT					0
#define GDT					1
//#define LDT				2

///////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel Memory Locations ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#define KERNEL_BASE			0x80000000
#define KERNEL_BASE_PHYS	0x9000
#define KERNEL_LOWMEM		0x80100000
	//kernel's virtual address for first 1MB of physical memory.
	// the additional 1MB is used for scratchpad (editpage) and kernel heap.

#define NUM_LOWMEM_PAGES	(2*MB/PAGE_SIZE)

// Kernel stack in boot is set to 80103FF0h, or phys 0x3FF0
#define BOOT_STACK			0x80103FF0
#define BOOT_STACK_PHYS		0x3000
#define USER_STACK			0x7FFFdFFC
#define USER_STACK_SIZE		(2*PAGE_SIZE)
#define KERNEL_STACK		0x7FFFfffc
#define KERNEL_STACK_SIZE	(1*KB)
#define KERNEL_CODE			KERNEL_BASE
#define USER_CODE			0x00000000

#define EDITPAGE_BASE		0x80200000
#define EDITPAGE_TEMP		0x80300000
#define KERNEL_HEAP			0x80201000

/* these boundaries give ~256MB for stack, 2GB-256MB for code/data/heap.*/
#define USER_BOUNDARY				KERNEL_BASE
	// Currently, this is the only 'boundary' value used in code.
#define STACK_BOUNDARY_LOW			0x70001000
	//must be >=
#define STACK_BOUNDARY_HIGH			KERNEL_BASE
	//must be <
#define HEAP_BOUNDARY				0x70000000
	//must be <
#define KERNEL_STACK_BOUNDARY_LOW	0x7ffff000
	//must be >=
#define KERNEL_STACK_BOUNDARY_HIGH	KERNEL_BASE
	//must be <

///////////////////////////////////////////////////////////////////////////////////////////////////
// Paging for IA-32 Architecture //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MEM_SIZE			(8*MB)
	//this will be gathered from CMOS info.
#define WORD_SIZE			4
	//in bytes.
#define BITMAP_SIZE			(MEM_SIZE/PAGE_SIZE/WORD_SIZE)
#define NUM_PAGES			(MEM_SIZE/PAGE_SIZE)
	//8K pages.
#define PAGE_SIZE			4096
#define PAGE_ADDRESS_SHIFT	12
#define PAGE_ADDRESS_MASK	((1 << PAGE_ADDRESS_SHIFT) -1)
#define PAGE_MASK			(~PAGE_ADDRESS_MASK)
#define NUM_PAGE_ENTRIES	1024

// IA-32 has D 6 | A 5 | PCD 4 | PWT 3 | U/s 2 | r/W 1 | P/np 0
// There is no bit in a page to specify it can be executed, in IA-32 this is set at the segment
// level, so assume all pages can be executed.
#define IA32_DIRTY			64
#define IA32_ACCESSED		32
#define IA32_PCD			16
#define IA32_PWT			8

#define IA32_USER_PAGE		4
#define IA32_KERNEL_PAGE	0
#define IA32_WRITE			2
#define IA32_READ			0
#define IA32_PRESENT		1
#define IA32_NOT_PRESENT	0

#define IA32_KERNEL_RW		(IA32_KERNEL_PAGE + IA32_WRITE + IA32_PRESENT)
#define IA32_USER_RW		(IA32_USER_PAGE + IA32_WRITE + IA32_PRESENT)

// Generate address of page directory/table entries using virtual address and base address of page directory/table.
#define gen_pd_index(virt_addr)		((virt_addr >> 22) * 4)
#define gen_pt_index(virt_addr)		(((virt_addr & 0x3FF000) >> 12) * 4)
#define user_page_entry(page_num)	((page_num << 12) | (IA32_USER_RW))
#define kernel_page_entry(page_num)	((page_num << 12) | (IA32_KERNEL_RW))

// page numbers for page directories and tables
#define KERNEL_PD_PAGE		4
#define KERNEL_PT0_PAGE		5
#define KERNEL_PT1_PAGE		6

// physical addresses for page directories and tables
#define PDk					(KERNEL_PD_PAGE*PAGE_SIZE)
#define PT0k				(KERNEL_PT0_PAGE*PAGE_SIZE)
#define PT1k				(KERNEL_PT1_PAGE*PAGE_SIZE)

// physical addresses for page directory/table entries
#define PDk_0				(PDk + gen_pd_index(0))
#define PDk_1				(PDk + gen_pd_index(KERNEL_BASE))
#define PT0k_0				(PT0k + gen_pt_index(0))
#define PT1k_0				(PT1k + gen_pt_index(KERNEL_BASE))
#define PT1k_STACK			(PT1k + gen_pt_index(BOOT_STACK))
		// this is messy, but for kernel stack before users start. The boot stack maps into
		// the kernel base's page table, so doesn't need its own entry.

///////////////////////////////////////////////////////////////////////////////////////////////////
// Boot Defintions ////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#define BOOT_SECTOR_PHYS	0x7C00
// phys address of boot sector.

#define BOOT_SECTOR_VIRT	(KERNEL_LOWMEM + BOOT_SECTOR_PHYS)
#define BLOCK_SIZE			1024
#define BLOCK_SHIFT			10
#define FILE_BLOCK_0		BOOT_SECTOR_VIRT
#define BOOT_TABLE_BASE		(FILE_BLOCK_0 + 4*BLOCK_SIZE)
#define BOOT_DATA_OFFSET	(4*4*5)

 // starts after boot table (5 entries of 4 int32_t)
#define BOOT_DATA_BASE		(BOOT_TABLE_BASE + BOOT_DATA_OFFSET)

#define SECTORS_REQD_OFFSET	0x05

///////////////////////////////////////////////////////////////////////////////////////////////////
// Boot Table Definition //////////////////////////////////////////////////////////////////////////
typedef struct boottable_entry
{
	int32_t size;
	int32_t block; //first block of object.
	int32_t priority;
	int32_t first_instruction;
} boot_table_t;

typedef unsigned user_boot_t;

#define BOOT_TABLE_PADDING				0x88
// This spells "boot".
#define BOOT_TABLE_OBJECT_STATUS_0		0x746f6f62
// This spells "tabl".
#define BOOT_TABLE_OBJECT_STATUS_1		0x6c626174

#define BOOT_TABLE_STATUS_0				0x12341234
#define BOOT_TABLE_FIRST_INSTRUCTION	0
// for all other entries...

#define BOOT_TABLE_STATUS_1				0x56785678

#define BOOT_TABLE_KERNEL_SIZE			BOOT_TABLE_BASE
#define BOOT_BLOCK_SIZE					(BOOT_DATA_BASE + 20)

/* Thase are the same addresses as above, except they are suitable for 16-bit physical addresses.*/
#define BOOT_TABLE_KERNEL_SIZE_16		(0xFFFF & BOOT_TABLE_BASE)
#define BOOT_BLOCK_SIZE_16				(0xFFFF & (BOOT_DATA_BASE + 20))

#endif

/* END OF FILE */
