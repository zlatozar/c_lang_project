///////////////////////////////////////////////////////////////////////////////////////////////////
/* Kernel Mechanisms Initialisation
	kernel_ia32.c

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @I web_mec_pc_intro_init Kernel Mechanisms Initialisation Code
// @G web_mec_pc_process_gate Process Mechanism - Gate Initialisation Code
// @P web_mec_pc_process_init Process Mechanism Initialisation Code
// @R web_mec_pc_resources_init Resources Mechanism Initialisation Code
// @M web_mec_pc_memory_segments2 Process Mechanisms Initialisation Code
// @D web_mec_pc_memory_init Memory Mechanism Initialisation Code for descriptors
// @G+
// @I+
// @P+
// @R+
// @D+

#include "../../policy/global.h"
#include "kernel_ia32.h"
#include "boot/bootdata.h"
// @G-
// @D-

#define FILESYS_PID		(1 + (1 << VERSION_SHIFT))
#define EXC_PID			(1 + (1 << VERSION_SHIFT))

///// Kernel Mechanism Interfaces /////////////////////////////////////////////////////////////////
// @P-
// @R-
void		InitBootParams(void);
int			InitialiseUsersMec(void);
// @P+
int			CreateProcessMec(void);
// @P-
// @R+
int			CreateResourceMec(void);
// @R-
// @D+
uint32_t	CreateMemoryMec(void);
// @D-

///// Kernel Mechanism Local Functions ////////////////////////////////////////////////////////////

int			LoadUser(int boot_entry);
int			CopyProcess(process_table_t* pID_ptr, uint32_t file_source, uint32_t file_size);
// @P+
// @D+
uint16_t	CreateTssDescriptor(int processT_entry);
// @P-
// @D-
int			CreateCallGates(void);
int			CreateInterruptGates(void);
// @P+
unsigned	CreateKernelTask(void);
// @D+
void		InitGDT(void);
// @P-
// @D-
uint16_t	CreateGate(unsigned char type, unsigned offset, uint16_t selector, int priv,
					   unsigned char word_count, unsigned table_base, uint16_t entry_no);
int			InitInterrupts(void);
int			InitClock(void);

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern pID_t AddProcess(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
						uint32_t user_stack_addr, uint32_t kernel_stack_addr,
						pID_t excepter_pID, pID_t pager_pID);
extern int   SetProcess(process_table_t* pID_ptr, int property, uint32_t value);

extern process_table_t* pIDtoPtr(pID_t pID);

extern int32_t	AddPage(process_table_t* pID_ptr, uint32_t virtual_address, uint32_t permissions);
extern int32_t	GrantPage(process_table_t* src_pID_ptr, uint32_t src_virtual_address,
						  process_table_t* dst_pID_ptr, uint32_t dst_virtual_address,
						  uint32_t permissions);

extern int	AddProcessMec(process_table_t* pID_ptr);

// @G+
// Imported Call Gates from kernel_pc.asm:
extern int	SendGate(), ReceiveGate(), ScheduleGate(), AddProcessGate(), RemoveProcessGate(),
			SetProcessGate(), GetProcessGate(), AddResourceGate(), RemoveResourceGate(),
			AddPageGate(), MapPageGate(), GrantPageGate(), UnmapPageGate();
// @G-
extern uint32_t	Out8(uint32_t port, uint8_t data);
extern uint8_t	In8(uint32_t port);
extern void		ReloadGDT(void);
extern void		LoadTaskRegister(uint16_t tss_selector);

// @G+
extern int	HwInt0(), HwInt1(), HwInt2(), HwInt3(), HwInt4(), HwInt5(), HwInt6(), HwInt7(),
			HwInt8(), HwInt9(), HwInt10(), HwInt11(), HwInt12(), HwInt13(), HwInt14(), HwInt15();

// exception entry points:
extern int	DivideErrorExc(), SingleStepExc(), NmiExc(), BpExc(), OverflowExc(), BoundsCheckExc(),
			InvalidOpcodeExc(), CoCpuExc(), DoubleFaultExc(), CoCpuOverrunExc(), InvalidTssExc(),
			SegNotPresentExc(), StackExc(), GenProtectionExc(), PageFaultExc(), CoCpuErrorExc();
// @G-

///// Local Data //////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel Mechanism Initialisation Interfaces /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void InitBootParams(void)
{
	int num_index_bits	= 0;
	int max_procs		= (*boot_max_processes) - 1;
	
	while (max_procs != 0) // convert max processes to bit size for index field.
	{
		max_procs = max_procs >> 1;
		num_index_bits++;
	}

	pID_transparent_mask = ~(((1 << *boot_transparent_bits) - 1)
							<< ((*boot_version_bits) + num_index_bits + (*boot_matching_bits)));
	
	return;
}

int InitialiseUsersMec(void)
{
	/* Idle and the Coordinator need to be loaded before the user system can take over.*/
	LoadUser(1); // Idle.
	LoadUser(2); // Coordinator.
	LoadUser(3); // Boot Device Driver.
	
	if (running_ptr == NULL)
	{
		running_ptr = idle_ptr;
		/* This ensures Schedule() selects the fast user, and because a different running_ptr is
		 * set, it forces a process switch. It does not mean that the Idle process is the first to
		 * run - Schedule() thinks that it has already run when actually it hasn't. */
		
		//ready_fastQ = ready_fastQ->next;
		/* If there are more than one initial user in the fast queue, can do this to ensure user2
		 * is selected first, as Schedule() selects the next and this will 'double' select ->next
		 * the first time, getting back to user2 (instead of user3) (Note: user1 is Idle).*/
	}
	return OK;
}
// @I-

// @P+
// @D+
int CreateProcessMec(void)
{
	InitGDT();
	CreateCallGates(); // initialise system calls.
	CreateInterruptGates(); // initialise low-level interrupt handlers.
	
	int processT_entry			= 0;
	process_table_t* pID_ptr	= first_process_ptr;
	
	/* The TSS selector used to switch processes requires m32:m16 field:
	 * uint32_t instruction pointer (EIP) : uint16_t TSS selector.
	 * The EIP value is actually irrelevant, so we just need to save the TSS selector to the second
	 * 32-bit word of the process mechanisms.
	 */
	while (pID_ptr < last_process_ptr)
	{
		pID_ptr->mechanisms[1] = CreateTssDescriptor(processT_entry++);
		pID_ptr++;
	}
	
	CreateKernelTask();
	
	return OK;
}
// @P-
// @D-

// @R+
int CreateResourceMec(void)
{
	//Interrupt Gates in IDT are initialised in CreateProcessMec() above.
	
	InitInterrupts(); // initialises PIC.
	InitClock(); //initialises main timer for scheduling.
	
	return OK;
}
// @R-

// @D+
uint32_t CreateMemoryMec(void)
{
// This creates the kernel page tables in IA-32.
	kernel_base_page	= KERNEL_PD_PAGE; //Physical base page for kernel.
	int i				= 0;

	/* Put Kernel Page Directory at physical address 0x4000, PT0k at 0x5000, PT1k at 0x6000.
	 * KERNEL_LOWMEM is the virtual address for physical low memory. Note: after first process
	 * switch, PDk and PT0k are not used (except as part of low memory). Only PT1k is used and is
	 * mapped into each process's page tables. */
	
	uint32_t* pagemem	= (uint32_t*)(PDk + gen_pd_index(KERNEL_LOWMEM));
	*pagemem			= PT1k | IA32_KERNEL_RW;
	
	/* Note that this maps PT for kernel code/base and low memory.
	 * pagemem = (uint32_t*)(PDk + gen_pd_index(KERNEL_BASE));
	 * *pagemem = PT1k | IA32_KERNEL_RW;*/

	/* 1MB low memory mapped to KERNEL_LOWMEM, for IDT, GDT, LDT and devices, eg CGA Video at
	 * 0xB.8000. Kernel page tables at 0x4000-0x6fff included in this.
	 * 256 (0x100) entries covers 1MB */
	
	pagemem = (uint32_t*)(PT1k + gen_pt_index(KERNEL_LOWMEM));
	
	// The first NUM_LOWMEM_PAGES physical pages assigned to kernel.
	for(i = 0; i < NUM_LOWMEM_PAGES; i++)
		*pagemem++ = i*PAGE_SIZE | IA32_KERNEL_RW;
		// This is the physical address of the new page, set with kernel privileges.
	
	// Assign pages for kernel code/data.
	pagemem				= (uint32_t*)(PT1k + gen_pt_index(KERNEL_BASE));
	boot_table_t *bootT	= (boot_table_t*)(BOOT_TABLE_BASE);
	int kernel_size		= (bootT->size * BLOCK_SIZE / PAGE_SIZE) + 1;
	
	for(i = 0; i < kernel_size; i++)
		*pagemem++ = (KERNEL_BASE_PHYS + i*PAGE_SIZE + IA32_KERNEL_RW);
	
	// Clear the rest of PT1k, avoiding the middle entries.
	pagemem = (uint32_t*)PT1k + kernel_size; //skip the code pages for the kernel.
	
	int num_pages_to_clear	= gen_pt_index(KERNEL_LOWMEM) / 4 - kernel_size;
	
	for(i = 0; i < num_pages_to_clear; i++)
		*pagemem++ = 0;
	
	// jump over the entries for low memory
	pagemem += NUM_LOWMEM_PAGES;
	
	int remaining_entries = NUM_PAGE_ENTRIES - NUM_LOWMEM_PAGES - gen_pt_index(KERNEL_LOWMEM)/4;
	
	for(i = 0; i < remaining_entries; i++)
		*pagemem++ = 0;
	
	// clear TLB:
	asm("movl $0x4000, %eax");
	asm("mov  %eax, %cr3");
	
	return OK;
}
// @D-

// @I+
///////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel Mechanism Local Functions ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int LoadUser(int boot_entry)
{
	boot_table_t *bootT; //boot table: kernel, idle, coord, filesys

	int i = 0;
	bootT = (boot_table_t*)(BOOT_TABLE_BASE);
	
	/* address of boot table is after boot+loader, i.e. 4KB after start & boot table is from
	 * physical address 0x8C00 to 0x9000 (Virtual address is 0x8010.8C00) */
	bootT += boot_entry;
	{
		uint32_t file_location		= bootT->block*KB + FILE_BLOCK_0;
		uint32_t first_instruction	= bootT->first_instruction;
		
		int32_t pID = AddProcess(bootT->priority, KERNEL_PID, first_instruction, USER_STACK,
								 KERNEL_STACK, EXC_PID, FILESYS_PID);
		
		process_table_t* pID_ptr;
		
		if (!(pID_ptr = pIDtoPtr(pID)))
			return ERROR_PID_PTR;
		
		CopyProcess(pID_ptr, file_location, bootT->size * KB);
		SetProcess(pID_ptr, P_READY, REASON_NEW);
	}
	return OK;
}

/* Copy a boot file from its position in memory, into new pages for the process.
 */
int CopyProcess(process_table_t* pID_ptr, uint32_t file_source, uint32_t file_size)
{
	int whole_pages		= file_size >> PAGE_ADDRESS_SHIFT;
	int remainder_bytes	= file_size & 0x0FFF;
	
	uint32_t *task_dest, *task_src;
	
	int i;
	int k = 0;
	
	task_src = (uint32_t*)(file_source);
	
	while (k < whole_pages)
	{
		AddPage(kernel_ptr, EDITPAGE_TEMP, USER_RWX);
		
		//copy relevant file system page to this new page.
		task_dest = (uint32_t*)(EDITPAGE_TEMP);
		
		for (i = 0; i < PAGE_SIZE / 4; i++)
			*task_dest++ = *task_src++;
		
		GrantPage(kernel_ptr, EDITPAGE_TEMP, pID_ptr, k*PAGE_SIZE, USER_RWX);
		
		k++;
	}
	if (remainder_bytes > 0)
	{
		AddPage(kernel_ptr, EDITPAGE_TEMP, USER_RWX);
		task_dest = (uint32_t*)(EDITPAGE_TEMP);
		
		for (i = 0; i < remainder_bytes / 4; i++)
			*task_dest++ = *task_src++;
		
		GrantPage(kernel_ptr, EDITPAGE_TEMP, pID_ptr, k*PAGE_SIZE, USER_RWX);
	}
	
	/* The file system is currently 3 pages long when running. Its binary image on disk
	 * is only 6.5KB, but its heap takes it over. As the file system is the primary
	 * pager, it cannot cause page faults, so the kernel must ensure it has enough pages
	 * assigned to it. This is a bit of a hack for the moment.*/
	
	if (pID_ptr->pID == 0x1001)
	{
		AddPage(pID_ptr, 0x1000, USER_RWX);
		AddPage(pID_ptr, 0x2000, USER_RWX);
		AddPage(pID_ptr, 0x3000, USER_RWX);
	}
	
	return OK;
}
// @I-

// @P+
// @M+
// @D+
/* Create a TSS descriptor for a given process.
 * This works by hardcoding a few values. Refer to the kernel mechanism documentation.
 */
uint16_t CreateTssDescriptor(int processT_entry)
{
	process_table_t* pID_ptr	= (process_table_t* )first_process_ptr + processT_entry;

	// base is the base address of the TSS
	uint32_t base				= (uint32_t)&pID_ptr->mechanisms[2];
	uint64_t *descriptor;
	int GDT_index				= (0x20 + processT_entry) * 8;
	
	/* Find an empty entry in descriptor table for this descriptor */
	descriptor	= (uint64_t*)(GDT_BASE + GDT_index);
	*descriptor	= (uint64_t)((base & 0xFF000000u)|((base & 0x00FF0000) >> 16) | 0x00008900);
	*descriptor	= (*descriptor << 32) | (base & 0xFFFF) << 16 | (TSS_SIZE - 1);
	
	return GDT_index;
}
// @P-
// @M-
// @D+

// @G+
int CreateCallGates(void)
{
	CreateGate(CALL_GATE_DESC, (unsigned)&SendGate,				CSk_SEL, PL3, 4, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&ReceiveGate,			CSk_SEL, PL3, 4, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&ScheduleGate,			CSk_SEL, PL3, 0, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&AddProcessGate,		CSk_SEL, PL3, 7, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&RemoveProcessGate,	CSk_SEL, PL3, 1, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&GetProcessGate,		CSk_SEL, PL3, 2, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&SetProcessGate,		CSk_SEL, PL3, 3, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&AddResourceGate,		CSk_SEL, PL3, 4, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&RemoveResourceGate,	CSk_SEL, PL3, 2, GDT_BASE, 0);
   	CreateGate(CALL_GATE_DESC, (unsigned)&AddPageGate,			CSk_SEL, PL3, 3, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&MapPageGate,			CSk_SEL, PL3, 5, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&GrantPageGate,		CSk_SEL, PL3, 5, GDT_BASE, 0);
	CreateGate(CALL_GATE_DESC, (unsigned)&UnmapPageGate,		CSk_SEL, PL3, 2, GDT_BASE, 0);

	return OK;
}

int CreateInterruptGates(void)
{
	/* All of this, including the CreateGate function, could be replaced by writing
	 * 0x0008iiii 0x80008e00 to each descriptor in the IDT, where iiii are the lower 4 nibbles
	 * of the ISR address. The following works - it needs to be redone so that all but the 4
	 * nibbles are written in a loop, and then the 4 nibbles for each one are written to 16-bits
	 * at regular offset in IDT. */
	
	uint32_t *descriptor;
	uint16_t *desc16;
	
	descriptor = (uint32_t*)(IDT_BASE);
	
	for(int i = 0; i < 0x200 / 8; i++)
	{
		*descriptor++ = 0x00080000;
		*descriptor++ = 0x80008e00u;
	}
	
	/* Exceptions */
	desc16  = (uint16_t*)(IDT_BASE);
	*desc16 = ((unsigned)DivideErrorExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)SingleStepExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)NmiExc) 			& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)BpExc) 			& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)OverflowExc) 		& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)BoundsCheckExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)InvalidOpcodeExc)	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)CoCpuExc) 			& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)DoubleFaultExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)CoCpuOverrunExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)InvalidTssExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)SegNotPresentExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)StackExc) 			& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)GenProtectionExc) 	& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)PageFaultExc) 		& 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)CoCpuErrorExc) 	& 0xFFFF;	desc16 += 4;

	/* Hardware Interrupts */
	desc16  = (uint16_t*)(IDT_BASE + 0x20*8);
	*desc16 = ((unsigned)HwInt0) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt1) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt2) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt3) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt4) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt5) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt6) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt7) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt8) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt9) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt10) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt11) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt12) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt13) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt14) & 0xFFFF;	desc16 += 4;
	*desc16 = ((unsigned)HwInt15) & 0xFFFF;	desc16 += 4;
	
	return OK;
}
// @G-

// @P+
// @M+
// @D+
unsigned CreateKernelTask(void)
{
	/* Note that the kernel task gets created in the process table's first slot which is used by
	 * Idle once the system is up and running. Idle should not be able to run first as its TSS will
	 * be marked busy from the kernel being the previous task. This is ensured by making its
	 * priority 0 compared with the coord and filesys.*/
	
	int kernel_task_entry	= (*boot_max_processes) - 1;
	
	uint16_t tss_selector	= CreateTssDescriptor(kernel_task_entry);//Kernel TSS
	
	AddProcessMec(first_process_ptr); //in process_ia32.c, creates TSS for kernel.
	
	LoadTaskRegister(tss_selector); //in kernel_pc.asm
	
	return OK;
}

void InitGDT(void)
{
	/* First 5 descriptors are defined by boot code, ie. null, kernel CS/DS, user CS/DS.
	 * Size of GDT is 64 entries * 8 bytes = 512 bytes.*/
	uint64_t *entry;
	entry = (uint64_t*) GDT_BASE+5; //address for entry 5
	
	for (int i = 0; i < (64 - 5); i++)
	{
		*entry = 0;
		entry++;
	}
	
	ReloadGDT(); //in kernel_pc.asm
}
// @P-
// @M-
// @D-

// @G+
/* Create a Gate of various types.
 * Type can be: task gate, call gate, interrupt gate, trap gate. Note: entry_no is used only for
 * IDT, where the position of insertion is explicit, rather than next empty location. Returns the
 * location of the descriptor entry in table. */
uint16_t CreateGate(unsigned char type, unsigned offset, uint16_t selector, int priv,
					unsigned char word_count, unsigned table_base, uint16_t entry_no)
{
	uint16_t		offset15_0, offset31_16, desc_return;
	unsigned char	access;
	uint64_t		*descriptor;
	unsigned		*desc_low, *desc_high, dret; // dret is temp for descriptor return value

	//P is set.
	offset15_0	= offset & 0xFFFF;
	// selector is next.
	// word count & 0x1F;
	access		= 0x80 | (priv << 5) | type;
	offset31_16	= offset >> 16;

	descriptor	= (uint64_t*) table_base;
	
	if (table_base == IDT_BASE)
	{
		descriptor += entry_no;
	}
	else
	{
		if (table_base == GDT_BASE)
			descriptor++;
		
		int entry_count = 0;
		
		// find an empty slot.
		while (*descriptor != 0 && entry_count < GDT_LIMIT)
		{
			descriptor++;
			entry_count++;
		}
	}
	desc_low	= (unsigned *) descriptor;
	*desc_low	= offset15_0 | (selector << 16);
	desc_high	= desc_low + 1;
	*desc_high	= (word_count & 0x1F) | (access << 8) | (offset31_16 << 16);
	dret		= (unsigned)desc_low;
	dret		= dret - table_base;
	desc_return	= (uint16_t)dret; // note this doesn't set TI bit or RPL.
	
	return desc_return;
}
// @G-

// @R+
/* InitInterrupts() remaps the PICs so that IRQ0 starts at 0x20 and IRQ8 starts at 0x28. Interrupts
 * are not enabled yet (ie Interrupt Flag (IF) is cleared).*/
int InitInterrupts(void)
{
	/* send ICW1 */
	/* D4=1 (ICW1_INIT) to specify initialisation command; IC4=1 means include ICW4;
	   edge-triggered (0); cascade mode (0). */
	Out8(PIC1_COMMAND, ICW1_INIT + IC4); //PIC1 = 0x20
	Out8(PIC2_COMMAND, ICW1_INIT + IC4); //PIC2 = 0xA0

	/* ICW2-4 are at PIC port address +1, ie PICn_DATA
	/* send ICW2, remap pics */
	Out8(PIC1_DATA, PIC1_START); //vectors start at 0x20
	Out8(PIC2_DATA, PIC2_START); //vectors start at 0x28

	/* send ICW3 */
	Out8(PIC1_DATA, 4); /* 1^n, where n=IRQ2 -> connection to slave */
	Out8(PIC2_DATA, 2); /* slave ID = 2. */

	/* send ICW4 */
	/* not-SFNM, non-buf, normal EOI ACK, processor mode PM=8086*/
	Out8(PIC1_DATA, NOT_SFNM + NONBUF + NORM_EOI + PM8086);
	Out8(PIC2_DATA, NOT_SFNM + NONBUF + NORM_EOI + PM8086);

	/* disable all IRQs */
 	Out8(PIC1_DATA, 0xFB); /* this leaves cascade IRQ2 unmasked. */
 	Out8(PIC2_DATA, 0xFF); /* mask all of PIC2's IRQs. */

	return OK;
}

int InitClock(void)
{
	/* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */
	uint8_t interrupt_mask;
	Out8(TIMER_MODE, SQUARE_WAVE);	/* set timer to run continuously */
	Out8(TIMER0, TIMER_COUNT);	/* load timer low byte */
	Out8(TIMER0, TIMER_COUNT >> 8);	/* load timer high byte */
	// enable its IRQ.
	interrupt_mask = In8(PIC1_DATA); // get current interrupt mask
 	interrupt_mask &= ~(1 << IRQ0); // set IRQ0 to be one, keep rest the same
 	Out8(PIC1_DATA, interrupt_mask); //set new interrupt mask in PIC
	
	return OK;
}
// @R-

/* END OF FILE */
