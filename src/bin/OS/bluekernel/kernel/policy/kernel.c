///////////////////////////////////////////////////////////////////////////////////////////////////
/* Kernel Initialisation
	kernel.c

	This file contains the main() entry-point from the kernel loader assembly
	code. This is 32-bit protected mode code. The main code performs:
	1. initialisation of system
	2. loads initial processes.
	3. runs scheduler.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_intro_kernel Kernel Initialisation Code
// @P web_process_init Process Initialisation Code
// @R web_resources_init Resources Initialisation Code
// @M web_memory_init Memory Initialisation Code
// @A+
// @P+
// @R+
// @M+

#include "global.h"
#include "mec.h"

///// Kernel Initialisation ///////////////////////////////////////////////////////////////////////

int CreateProcessManager(void);
int CreateMessageManager(void);
int CreateResourceManager(void);
int CreateMemoryManager(void);
// @P-
// @R-
// @M-

///// Kernel Local Functions //////////////////////////////////////////////////////////////////////

///// External Interfaces /////////////////////////////////////////////////////////////////////////

///// Policy Interfaces ///////////////////////////////////////////////////////////////////////////

extern int Schedule(void);

///// Local Data //////////////////////////////////////////////////////////////////////////////////
// @P+
// @R+
// @M+
///////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel Initialisation Interfaces ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int kmain(void)
{
	CreateMemoryManager();
	
	#if KERNEL_DEBUG
	debug_count		= 0;
	exception_count	= 0;
	interrupt_count	= 0;
	wakeup_count	= 0;
	#endif
	
	InitBootParams();
	CreateProcessManager();
	CreateMessageManager();
	LockBus();
	/* The bus is locked because the clock interrupt is enabled by
	 * CreateResourceManager(), but we want to wait until Schedule() is called before
	 * the clock triggers.*/
	CreateResourceManager();

	/* The kernel has to load in the initial user processes that start everything else.*/
	InitialiseUsersMec();
	
	#if VIDEO_DEBUG
	VideoInit(); // This is for debugging only - video is removed from final kernel.
	VideoProcessUpdate(); // Update the display of the process table.
	#endif
	
	UnlockBus();
	Schedule(); //This invocation of Schedule() never returns.
}
// @R-
// @M-

int CreateProcessManager(void)
{
	num_processes	= 0;
	current_pID		= KERNEL_PID;
	readyQ			= NULL;
	ready_fastQ		= NULL;
	blockedQ		= NULL;
	running_ptr		= NULL;
	
	// for debugging, align the process table to 0x1000 boundary.
	#if KERNEL_DEBUG
	unsigned khn_temp	= (unsigned)kernel_heap_next;
	khn_temp			= (khn_temp & 0xFFFFF000) + 0x1000;
	kernel_heap_next	= (unsigned*)khn_temp;
	#endif

	processT			= (process_table_t*)kernel_heap_next;
    first_process_ptr	= processT;
    last_process_ptr	= (first_process_ptr + *boot_max_processes);
	/* NB: last_process_ptr points beyond the process table, not at the last process.*/
	
    kernel_heap_next	= (unsigned*)last_process_ptr;
	idle_ptr			= first_process_ptr;
	max_pID				= (*boot_max_processes << VERSION_SHIFT) - 1;
	num_schedules		= 0;
	coord_set			= 0;
	
	#if KERNEL_DEBUG
	sch_log_index = 0;
	#endif
	
	// Initialise the process table.
	int processT_entry			= 0;
	process_table_t* pID_ptr	= first_process_ptr;
	
	while (pID_ptr < last_process_ptr)
	{
		pID_ptr->pID		= processT_entry << VERSION_SHIFT;
		pID_ptr->status		= EMPTY;
		pID_ptr->reason		= 0;
		pID_ptr->own_msgQn	= 0;
		pID_ptr->own_msgQp	= 0;
		pID_ptr->wait_msgQn	= 0;
		pID_ptr->wait_msgQp	= 0;
		pID_ptr->next		= 0;
		pID_ptr->prev		= 0;
		pID_ptr->base_page	= 0;
		pID_ptr->child		= 0;
		pID_ptr->sibling	= 0;
		
		#if KERNEL_DEBUG
		// the dummy values are used to clearly mark the end of a process entry.
		int i;
		for (i = 0; i < NUM_DUMMIES; i++)
			pID_ptr->dummy[i] = 0xFFFFFFFF;
		#endif

		pID_ptr++;
		processT_entry++;
	}
	CreateProcessMec();
	
	return OK;
}
// @P-

int CreateMessageManager(void)
{
	return OK;
}

// @R+
int CreateResourceManager(void)
{
	int i;
	num_sleepers		= 0;
	num_errors			= 0;
	num_check_failures	= 0;
	clock_schedule		= 0;
	clock_seconds		= 0;
	clock_ticks			= 0;
	prev_ticks			= 0;
	
	#if KERNEL_DEBUG
	wakeup7 = 0; //debug for stray interrupts.
	for (i = 0; i < (EXCEPTION_LAST - EXCEPTION_FIRST); i++)
		exception_log[i] = 0;
	#endif

	interruptT			= (interrupt_table_t*)kernel_heap_next;
	kernel_heap_next	= (unsigned*)&interruptT[NUM_INTERRUPTS];
	
	portT				= (port_table_t*)kernel_heap_next;
	kernel_heap_next	= (unsigned*)(portT + *boot_num_ioports);
	
	for(i = 0; i < NUM_IOPORTS; i++)
	{
		portT[i].port	= 0;
		portT[i].pID	= 0;
	}
	
	for(i = 0; i < NUM_INTERRUPTS; i++)
	{
		#if KERNEL_DEBUG
		interruptT[i].int_num		= i;
		interruptT[i].dummy[0]		= 0x11223344;
		interruptT[i].dummy[1]		= 0x55667788;
		#endif

		interruptT[i].pID			= 0;
		interruptT[i].own_msgQn		= (process_table_t*)(i + INTERRUPT_OFFSET);
		interruptT[i].own_msgQp		= (process_table_t*)(i + INTERRUPT_OFFSET);
		interruptT[i].wakeups		= 0;
		interruptT[i].occurrences	= 0;
	}
	
	/* The clock is effectively automatically registered by the kernel, so its message
	 * queue pointers need to be set here instead of in AddResource() as with all other
	 * interrupts.*/
	//interruptT[CLOCK_INTERRUPT - INTERRUPT_OFFSET].own_msgQn = (process_table*)CLOCK_INTERRUPT;
	//interruptT[CLOCK_INTERRUPT - INTERRUPT_OFFSET].own_msgQp = (process_table*)CLOCK_INTERRUPT;
	
	CreateResourceMec();
	
	#if KERNEL_DEBUG
	// The following debugging fields are created after everything else in kernel init.
	schedule_log		= (int32_t*)kernel_heap_next;
	kernel_heap_next	= (unsigned*)&schedule_log[SCHEDULE_LOG_MAX];
	
	for (i = 0; i < SCHEDULE_LOG_MAX; i++)
		schedule_log[i] = 0;
	
	message_log			= (int32_t*)kernel_heap_next;
	kernel_heap_next	= (unsigned*)&message_log[MSG_LOG_MAX];
	msg_log_index		= 0;
	
	for (i = 0; i < MSG_LOG_MAX; i++)
		message_log[i] = 0;
	
	#endif

	return OK;
}
// @R-

// @M+
int CreateMemoryManager(void)
{
	CreateMemoryMec();
	kernel_heap_next	= (unsigned*)KERNEL_HEAP;
	memory_size			= GetMemorySize();
	num_pages			= memory_size/PAGE_SIZE;
	page_ownersT		= (page_owners_t*)kernel_heap_next;
	kernel_heap_next	= (unsigned*)(page_ownersT + num_pages);
	
	int i;
	for (i = 0; i < num_pages; i++)
	{
		page_ownersT[i].next_page = i+1;
		page_ownersT[i].page_maps = EMPTY_PAGE_MAP;
	}
	
	//indicates last in list for free pages.
	page_ownersT[i - 1].next_page					= NULL_PAGE;
	
	/* The first 0x200 (2MB, NUM_LOWMEM_PAGES) pages are for the kernel. The mechanism decides
	 * where these pages are mapped. The following works so long as 'num_pages' is big enough.*/
	page_ownersT[NUM_LOWMEM_PAGES - 1].next_page	= NULL_PAGE; //end.
	
	next_free_page		= NUM_LOWMEM_PAGES;
	num_free_pages		= num_pages - NUM_LOWMEM_PAGES;

	page_mapsT			= (page_maps_t*)kernel_heap_next;
	kernel_heap_next	= (unsigned*)(page_mapsT + *boot_num_page_maps);
	
	for(i = 0; i < *boot_num_page_maps; i++)
	{
		page_mapsT[i].map_num		= i;
		page_mapsT[i].dummy[0]		= 0x11111111;
		page_mapsT[i].dummy[1]		= 0x11111111;
		page_mapsT[i].physical_page	= 0;
		page_mapsT[i].virtual_page	= 0;
		page_mapsT[i].next_pID		= i + 1;
		page_mapsT[i].next_phys		= EMPTY_PAGE_MAP;
		page_mapsT[i].pID_ptr		= 0;
	}
	
	page_mapsT[i - 1].next_pID		= EMPTY_PAGE_MAP;
	next_free_map					= 0;
	num_free_maps					= NUM_MAPS;
	
	return OK;
}
// @M-
// @A-

#if KERNEL_DEBUG
int Error(int code)
{
	/* this is used for debugging: a breakpoint can be set for this function and when it is entered
	 * the return value can be checked to see where the error came from in the code. */
	return code;
}
#endif

/* END OF FILE */
