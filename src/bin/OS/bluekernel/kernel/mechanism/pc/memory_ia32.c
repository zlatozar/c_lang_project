///////////////////////////////////////////////////////////////////////////////////////////////////
/* Memory Mechanisms for IA-32
	memory_ia32.c

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @P web_mec_pc_memory_permissions Memory Mechanisms Permission Code
// @T web_mec_pc_memory_tlb Memory Mechanisms for TLB and scratchpad
// @P+
// @T+

#include "kernel_ia32.h"
#include "../../policy/global.h"

///// Memory Mechanism Interfaces /////////////////////////////////////////////////////////////////
// @P-
// @T-
int			AddMemoryMec(process_table_t* pID_ptr);
// @P+
int			AddPageMec(process_table_t* pID_ptr, int32_t physical_page, uint32_t virtual_address,
					   uint32_t permissions);
// @P-
int32_t		RemovePageMec(process_table_t* pID_ptr, uint32_t virtual_address);
// @T+
void		EditPageMec(uint32_t physical_page);
void		ClearPageMec(void);
// @T-
uint32_t	GetMemoryMec(process_table_t* pID_ptr, int property, uint32_t virtual_address);
uint32_t	GetPhysAddrMec(process_table_t* pID_ptr, uint32_t virtual_address);
// @P+
uint32_t	GetPermissionsMec(process_table_t* pID_ptr, uint32_t virtual_address);
// @P-
uint32_t	GetMemorySize(void);

// @P+
///// Memory Mechanism Local Functions ////////////////////////////////////////////////////////////

uint32_t	AddPageTableEntryMec(uint32_t physical_page, uint32_t index, uint32_t permissions);
// @P-
int32_t		GetPageTableMec(uint32_t virtual_address);

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern int32_t	AppendNewPage(int32_t j);

extern uint8_t	In8(uint32_t port);
extern uint32_t	Out8(uint32_t port, uint8_t data);
extern void		Flush(uint32_t virtual_address);

///// Local Data //////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Memory Mechanism Interfaces /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int AddMemoryMec(process_table_t* pID_ptr)
{
	EditPageMec(pID_ptr->base_page);
	ClearPageMec();
	AddPageTableEntryMec(KERNEL_PT1_PAGE, gen_pd_index(KERNEL_CODE), KERNEL_RWX); //write PT1k PDE
	
	return OK;
}

// @P+
int AddPageMec(process_table_t* pID_ptr, int32_t physical_page, uint32_t virtual_address, uint32_t permissions)
{
	int32_t		page_table;
	uint32_t	base_page, existing_page;
	
	if (pID_ptr != kernel_ptr)
		base_page = pID_ptr->base_page;
	else
		base_page = kernel_base_page;
	
	EditPageMec(base_page);
	page_table = GetPageTableMec(virtual_address);
	
	// need to test whether page_table is 0, and make PT if so.
	if (!page_table) //create page table.
	{
		page_table = AppendNewPage(base_page);
		if (!page_table)
			return -1;
		
		//write PDE for new PT:
		existing_page = AddPageTableEntryMec(page_table,gen_pd_index(virtual_address),USER_RWX);
		
		if (existing_page != 1)
			return -1;
		
		EditPageMec(page_table);
		ClearPageMec();
	}
	else
	{
		EditPageMec(page_table);
	}
	
	existing_page = AddPageTableEntryMec(physical_page,gen_pt_index(virtual_address),permissions);
	
	if (existing_page != 1)
		return -1;
	
	return OK;
}
// @P-

int32_t RemovePageMec(process_table_t* pID_ptr, uint32_t virtual_address)
{
	int32_t		k, physical_page;
	uint32_t	*scratchpad;
	
	if (pID_ptr != kernel_ptr)
		EditPageMec(pID_ptr->base_page);
	else
		EditPageMec(kernel_base_page);
	
	k = GetPageTableMec(virtual_address);
	
	if (!k)
	{
		return -1;
	}
	else
	{
		EditPageMec(k); //edit PT.
		scratchpad = (uint32_t*)(EDITPAGE_BASE + gen_pt_index(virtual_address));
		
		if (*scratchpad == 0)
			return -1; // virt page does not exist.
		
		physical_page	= *scratchpad >> 12;
		*scratchpad		= 0;
		
		Flush(virtual_address);//Invalidate TLB entry
		
		return physical_page;
	}
}

// @T+
void EditPageMec(uint32_t physical_page)
{
	uint32_t *scratchpad;
	asm("invlpg 0x80200000"); //EDITPAGE_BASE
	scratchpad	= (uint32_t*) 0x80106800; // kernel page-table entry for 0x80200000
	*scratchpad	= user_page_entry(physical_page);
}

/* ClearPageMec() clears the page currently set by EditPageMec().*/
void ClearPageMec(void)
{
	uint32_t *scratchpad;
	scratchpad = (uint32_t*)EDITPAGE_BASE;
	
	for (int i = 0; i < NUM_PAGE_ENTRIES; i++)
		*scratchpad++ = 0;
	
	return;
}
// @T-

uint32_t GetMemoryMec(process_table_t* pID_ptr, int property, uint32_t virtual_address)
{
	if (pID_ptr != kernel_ptr)
		EditPageMec(pID_ptr->base_page);
	else
		EditPageMec(kernel_base_page);
	
	uint32_t page_table = GetPageTableMec(virtual_address);
	
	EditPageMec(page_table);
	
	uint32_t *page_table_entry, settings;
	
	page_table_entry = (uint32_t*)(EDITPAGE_BASE + gen_pt_index((virtual_address)));
	
	// IA-32 has D 6 | A 5 | PCD 4 | PWT 3 | U/s 2 | r/W 1 | P/np 0
	// Refer to kernel_ia32.h
	// There is no bit in a page to specify it can be executed, in IA-32 this
	// is set at the segment level, so assume all pages can be executed.
	
	if ((*page_table_entry & IA32_WRITE)==IA32_WRITE)
		settings = USER_RWX;
	else
		settings = USER_R_X;
	
	if ((*page_table_entry & IA32_PRESENT)==IA32_PRESENT)   	settings += PAGE_PRESENT;
	if ((*page_table_entry & IA32_USER_PAGE)==IA32_KERNEL_PAGE)	settings += KERNEL_PAGE;
	if ((*page_table_entry & IA32_ACCESSED)==IA32_ACCESSED)		settings += PAGE_ACCESSED;
	if ((*page_table_entry & IA32_DIRTY)==IA32_DIRTY)			settings += PAGE_DIRTY;
	
	/* property is a mask, eg use KERNEL_RWX to return value for each of these, or use PAGE_DIRTY
	 * to see if the page has been modified.*/
	settings &= property;
	
	return settings;
}

uint32_t GetPhysAddrMec(process_table_t* pID_ptr, uint32_t virtual_address)
{
	if (pID_ptr != kernel_ptr)
		EditPageMec(pID_ptr->base_page);
	else
		EditPageMec(kernel_base_page);
	
	uint32_t page_table = GetPageTableMec(virtual_address);
	
	if (page_table == 0)
		return 0;
	
	EditPageMec(page_table);
	
	uint32_t *page_table_entry, phys_addr;
	
	page_table_entry = (uint32_t*)(EDITPAGE_BASE + gen_pt_index((virtual_address)));
	
	if (*page_table_entry == 0)
		return 0;
	
	//this gets address of phys page no. therefore *scratchpad contains phys page no.
	phys_addr = (*page_table_entry & 0xFFFFF000) | (virtual_address & 0xFFF);
	/* This takes the phys page no and blanks out the low 12-bits of the PTE, and ORs it with the
	 * low 12 bits of the virtual address, which form the page offset within the virtual address.*/
	
	return phys_addr;
}

// @P+
uint32_t GetPermissionsMec(process_table_t* pID_ptr, uint32_t virtual_address)
{
	if (pID_ptr != kernel_ptr)
		EditPageMec(pID_ptr->base_page);
	else
		EditPageMec(kernel_base_page);
	
	uint32_t page_table = GetPageTableMec(virtual_address);
	
	EditPageMec(page_table);
	
	uint32_t *page_table_entry, permissions;
	
	page_table_entry = (uint32_t*)(EDITPAGE_BASE + gen_pt_index((virtual_address)));
	
	// IA-32 has U/s | r/W | P/np
	// There is no bit in a page to specify it can be executed, in IA-32 this is set at the segment
	// level, so assume all pages can be executed.
	if ((*page_table_entry & 0x2) == 0)
		permissions = USER_R_X;
	else
		permissions = USER_RWX;
	
	if ((*page_table_entry & 0x4) == 0)
		permissions += KERNEL_PAGE;
	
	return permissions;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Memory Mechanism Local Functions ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/* Adds a page table entry where physical_page is the page table, and index is the
entry within this table. *scratchpad is used to point to this entry and write the value
of the PTE there. The scratchpad should already be set to edit this physical page
using EditPageMec(physical_page).
page table entry format:
page base address | other bits | U/s | r/W | P/np
U/s: user/supervisor(kernel): 1=U, 0=S
r/W: read/write: 1=Write (ie RW), 0=Read
[P/np: Present/not-present - these don't concern us here]
*/
uint32_t AddPageTableEntryMec(uint32_t physical_page, uint32_t index, uint32_t permissions)
{
	uint32_t *scratchpad, permission_bits;
	
	scratchpad = (uint32_t*)(EDITPAGE_BASE + index);
	
	if (*scratchpad != 0)
		return *scratchpad;
	
	if (permissions & KERNEL_PAGE)
		permission_bits = 1;
	else
		permission_bits = 5;
	
	if (permissions & WRITE)
		permission_bits += 2;
	
	*scratchpad = (physical_page << 12) | permission_bits;
	
	return OK;
}
// @P-

/* GetPageTableMec() returns the physical page number of the page table for virtual_address.*/
int32_t GetPageTableMec(uint32_t virtual_address)
{
	uint32_t *scratchpad;

	scratchpad = (uint32_t*)(EDITPAGE_BASE + gen_pd_index(virtual_address));

	return (*scratchpad >> 12);
}

/* Determining the installed memory of a system is considerably more difficult and varied than
 * what's done here. This reads from the CMOS registers, and may miss the 15MB 'hole'. It doesn't
 * do too badly though. Another option is to use BIOS functions in the boot code.*/
uint32_t GetMemorySize(void)
{
	uint32_t		total;
	unsigned char	lowmem, highmem;

	Out8(0x70, 0x30);
	lowmem	= In8(0x71);
	Out8(0x70, 0x31);
	highmem	= In8(0x71);
	total	= (lowmem | highmem << 8) + 1024;
	total	*= 1024;
	
	return total;
}

/* END OF FILE */
