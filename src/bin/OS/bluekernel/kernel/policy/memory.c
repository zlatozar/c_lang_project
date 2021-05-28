///////////////////////////////////////////////////////////////////////////////////////////////////
/* Memory Management
	memory.c

	This implements:
	1. Creation and destruction of address space for user processes
	2. adding pages to an address space
	3. mapping pages
	4. granting pages from one address space to another

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_memory_add Memory Add/Remove Code
// @P web_memory_paging Memory Paging Code
// @A+
// @P+

#include "global.h"
#include "mec.h"

///// Memory Management Interfaces ////////////////////////////////////////////////////////////////
// @P-

int32_t		AddMemory(process_table_t* pID_ptr);
uint32_t	RemoveMemory(process_table_t* pID_ptr);
// @A-

int32_t		AddPageCall(pID_t pID, uint32_t virtual_address, uint32_t permissions);
int32_t		MapPageCall(pID_t src_pID, uint32_t src_virtual_address,
					    pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions);
int32_t		GrantPageCall(pID_t src_pID, uint32_t src_virtual_address,
						  pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions);
int32_t		UnmapPageCall(pID_t pID, uint32_t virtual_address);

// @P+
int32_t		AddPage(process_table_t* pID_ptr, uint32_t virtual_address, uint32_t permissions);
int32_t		MapPage(process_table_t* src_pID_ptr, uint32_t src_virtual_address,
					process_table_t* dst_pID_ptr, uint32_t dst_virtual_address, uint32_t permissions);
int32_t		GrantPage(process_table_t* src_pID_ptr, uint32_t src_virtual_address,
					  process_table_t* dst_pID_ptr, uint32_t dst_virtual_address,
					  uint32_t permissions);
int32_t		UnmapPage(process_table_t* pID_ptr, uint32_t virtual_address);

///// Memory Management Local Functions ///////////////////////////////////////////////////////////

int32_t		AppendNewPage(int32_t j);
int			RemoveMappings(uint32_t physical_page);
int32_t		CheckPageOwner(process_table_t* pID_ptr, int32_t physical_page);
int32_t		CheckRange(process_table_t* pID_ptr, uint32_t virtual_address);
// @P-

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern process_table_t* pIDtoPtr(pID_t pID);
extern int Check(void);

///// Local Data //////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Memory Management System Call Interfaces ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int32_t AddPageCall(pID_t pID, uint32_t virtual_address, uint32_t permissions)
{
	LockBus();
	running_ptr->last_syscall = ADD_PAGE_SYSCALL;
	
	if (Check() < OK)
	{
		UnlockBus();
		return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
	}
	
	process_table_t* pID_ptr;
	
	if (pID == USE_CURRENT_PROCESS)
	{
		pID		= current_pID;
		pID_ptr	= running_ptr;
	}
	else if (!(pID_ptr = pIDtoPtr(pID)))
	{
		UnlockBus();
		return ERROR_PID_PTR;
	}
	
	int32_t result = AddPage(pID_ptr, virtual_address, permissions);
	UnlockBus();
	
	return result;
}

int32_t MapPageCall(pID_t src_pID, uint32_t src_virtual_address,
					pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions)
{
	LockBus();
	running_ptr->last_syscall = MAP_PAGE_SYSCALL;
	
	if (Check() < OK)
	{
		UnlockBus();
		return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
	}
	
	process_table_t* src_pID_ptr;
	
	if (src_pID == USE_CURRENT_PROCESS)
	{
		src_pID_ptr = running_ptr;
	}
	else if (!(src_pID_ptr = pIDtoPtr(src_pID)))
	{
		UnlockBus();
		return ERROR_PID_PTR;
	}

	process_table_t* dst_pID_ptr;
	
	if (dst_pID == USE_CURRENT_PROCESS)
	{
		dst_pID_ptr = running_ptr;
	}
	else if (!(dst_pID_ptr = pIDtoPtr(dst_pID)))
	{
		UnlockBus();
		return ERROR_PID_PTR;
	}

	int32_t result = MapPage(src_pID_ptr, src_virtual_address,
							 dst_pID_ptr, dst_virtual_address, permissions);
	UnlockBus();
	
	return result;
}

int32_t GrantPageCall(pID_t src_pID, uint32_t src_virtual_address,
					  pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions)
{
	LockBus();
	running_ptr->last_syscall = GRANT_PAGE_SYSCALL;
	
	if (Check() < OK)
	{
		UnlockBus();
		return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
	}
	
	process_table_t* src_pID_ptr;
	
	if (src_pID == USE_CURRENT_PROCESS)
	{
		src_pID_ptr = running_ptr;
	}
	else if (!(src_pID_ptr = pIDtoPtr(src_pID)))
	{
		UnlockBus();
		return ERROR_PID_PTR;
	}
	
	process_table_t* dst_pID_ptr;
	
	if (dst_pID == USE_CURRENT_PROCESS)
	{
		dst_pID_ptr = running_ptr;
	}
	else if (!(dst_pID_ptr = pIDtoPtr(dst_pID)))
	{
		UnlockBus();
		return ERROR_PID_PTR;
	}
	
	int32_t result = GrantPage(src_pID_ptr, src_virtual_address,
							   dst_pID_ptr, dst_virtual_address, permissions);
	UnlockBus();
	return result;
}

int32_t UnmapPageCall(pID_t pID, uint32_t virtual_address)
{
	LockBus();
	running_ptr->last_syscall = UNMAP_PAGE_SYSCALL;
	
	if (Check() < OK)
	{
		UnlockBus();
		return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
	}
	
	process_table_t* pID_ptr;
	
	if (pID == USE_CURRENT_PROCESS)
	{
		pID		= current_pID;
		pID_ptr	= running_ptr;
	}
	else if (!(pID_ptr = pIDtoPtr(pID)))
	{
		UnlockBus();
		return ERROR_PID_PTR;
	}

	int32_t result = UnmapPage(pID_ptr, virtual_address);
	UnlockBus();
	
	return result;
}

// @A+
///////////////////////////////////////////////////////////////////////////////////////////////////
///// Memory Management Interfaces ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int32_t AddMemory(process_table_t* pID_ptr)
{
	// Allocate Address Space, where 3 pages is minimum required for new process.
	if (num_free_pages < 3)
		return ERROR_NO_FREE_PAGES;
		
	pID_ptr->base_page	= next_free_page;
	next_free_page		= page_ownersT[next_free_page].next_page;
	
	page_ownersT[pID_ptr->base_page].next_page = NULL_PAGE;
	num_free_pages--;
	
	//Need to clear root PT and map kernel page tables.
	AddMemoryMec(pID_ptr);
	
	return OK;
}

uint32_t RemoveMemory(process_table_t* pID_ptr)
{
	/*Remove memory for process. Simply point next free page to first
	page of process, and last page assigned to process to old next free
	page.*/
	int32_t old_free_page	= next_free_page;
	int32_t k				= pID_ptr->base_page;
	
	int j, map_k, map_j;
	
	while (page_ownersT[k].next_page != NULL_PAGE)
	{
		RemoveMappings(k); //remove any mappings associated with this process.
		k = page_ownersT[k].next_page;
		num_free_pages++;
	}
	page_ownersT[k].next_page = old_free_page;

	//Need to remove page maps for this process
	map_k			= pID_ptr->page_maps;
	int map_count	= 0;
	map_j			= map_k;
	
	while (map_k != EMPTY_PAGE_MAP)
	{
		// get phys for map, and skip this entry on phys map list.
		int phys	= page_mapsT[map_k].physical_page;
		j			= page_ownersT[phys].page_maps;
		
		if (j == map_k) // need to skip map_k on phys list:
		{
			page_ownersT[phys].page_maps = page_mapsT[j].next_phys;
		}
		else
		{
			while (page_mapsT[j].next_phys != map_k)
				j = page_mapsT[j].next_phys;
			
			page_mapsT[j].next_phys = page_mapsT[map_k].next_phys; //jump over map_k
		}
		
		// clear everything to do with map_k
		RemovePageMec(pID_ptr, page_mapsT[map_k].virtual_page << PAGE_ADDRESS_SHIFT);
		
		page_mapsT[map_k].physical_page	= 0;
		page_mapsT[map_k].virtual_page	= 0;
		page_mapsT[map_k].pID_ptr		= 0;
		map_count++;
		map_j = map_k;
		map_k = page_mapsT[map_k].next_pID;
	}
	// when all of these maps are removed, need to assign maps to free map list
	if (map_j != EMPTY_PAGE_MAP)
	{
		page_mapsT[map_j].next_pID	= next_free_map;
		next_free_map				= pID_ptr->page_maps;
		pID_ptr->page_maps			= 0;
		num_free_maps				+= map_count;
	}
	return OK;
}
// @A-

// @P+
int32_t AddPage(process_table_t* pID_ptr, uint32_t virtual_address, uint32_t permissions)
{
	/* Need to find last page in list of pages assigned to pID, then
	add next free page to this list and set it to 0; update next free
	page; Then call mechanism to add this new page to page tables.*/
	
	int32_t new_physical_page;
	
	if (pID_ptr != kernel_ptr)
	{
		int32_t getphys = GetPhysAddrMec(pID_ptr, virtual_address);
		
		if (getphys != 0)
		{
			#if VIDEO_DEBUG
// 			VideoAddPage(current_pID, pID_ptr->pID, virtual_address, getphys);
			#endif
			return ERROR_PAGE_ALREADY_MAPPED;
		}
		if (!CheckRange(pID_ptr, virtual_address))
			return ERROR_ADDR_RANGE;
		
		new_physical_page = AppendNewPage(pID_ptr->base_page);
	}
	else
		new_physical_page = AppendNewPage(kernel_base_page);
	
	if (!new_physical_page)
		return ERROR_NO_PHYSICAL_PAGE;
	
	AddPageMec(pID_ptr, new_physical_page, virtual_address, permissions);
	EditPageMec(new_physical_page);
	ClearPageMec();
	
	if (pID_ptr != kernel_ptr)
		pID_ptr->size += PAGE_SIZE;
	
	return new_physical_page;
}

int32_t MapPage(process_table_t* src_pID_ptr, uint32_t src_virtual_address,
				process_table_t* dst_pID_ptr, uint32_t dst_virtual_address, uint32_t permissions)
{
	if (num_free_maps <= 0)
		return ERROR_NO_FREE_MAPS;
	
	if (!CheckRange(dst_pID_ptr, dst_virtual_address))
		return ERROR_ADDR_RANGE;
	
	uint32_t physical_address = GetPhysAddrMec(src_pID_ptr, src_virtual_address);
	permissions &= GetMemoryMec(src_pID_ptr, KERNEL_RWX, src_virtual_address);
	
	// Check that dst virt addr not mapped already.
	int32_t getphys = GetPhysAddrMec(dst_pID_ptr, dst_virtual_address);
	
	if (getphys != 0)
	{
		#if VIDEO_DEBUG
		VideoAddPage(current_pID, dst_pID_ptr->pID, dst_virtual_address, getphys);
		#endif
		return ERROR_PAGE_ALREADY_MAPPED;
	}
	
	int32_t physical_page	= physical_address>>PAGE_ADDRESS_SHIFT;
	
	/* Need to append a new page map entry to the process map list pointed to by k. The new page
	 * must be added to process map list, and phys map list.*/
	int32_t k						= dst_pID_ptr->page_maps;
	int32_t new_map					= dst_pID_ptr->page_maps = next_free_map;
	
	int32_t j						= page_mapsT[new_map].next_pID;
	page_mapsT[new_map].next_pID	= k; // old dst_pID_ptr->page_maps;
	next_free_map					= j;
	
	//append to phys map list:
	k										= page_ownersT[physical_page].page_maps;
	page_ownersT[physical_page].page_maps	= new_map;
	page_mapsT[new_map].next_phys			= k;
	num_free_maps--;
	page_mapsT[new_map].pID_ptr				= dst_pID_ptr;
	page_mapsT[new_map].physical_page		= physical_page;
	page_mapsT[new_map].virtual_page		= dst_virtual_address>>PAGE_ADDRESS_SHIFT;
	
	AddPageMec(dst_pID_ptr, physical_page, dst_virtual_address, permissions);
	
	dst_pID_ptr->size						+= PAGE_SIZE;
	
	return physical_page;
}

int32_t GrantPage(process_table_t* src_pID_ptr, uint32_t src_virtual_address,
				  process_table_t* dst_pID_ptr, uint32_t dst_virtual_address,
				  uint32_t permissions)
{
	if (!CheckRange(src_pID_ptr, src_virtual_address))
		return ERROR_ADDR_RANGE;
	
	if (!CheckRange(dst_pID_ptr, dst_virtual_address))
		return ERROR_ADDR_RANGE;

	if (dst_pID_ptr != kernel_ptr)
	{
		if (GetPhysAddrMec(dst_pID_ptr, dst_virtual_address) != 0)
			return ERROR_PAGE_ALREADY_MAPPED;
	}
	
	permissions &= GetMemoryMec(src_pID_ptr, KERNEL_RWX, src_virtual_address);
	
	int32_t physical_page = RemovePageMec(src_pID_ptr, src_virtual_address);
	
	if (CheckPageOwner(src_pID_ptr, physical_page) < OK || Check() < OK)
		return ERROR_SRC_NOT_PAGE_OWNER;

	// remove physical_page from src page owner list:
	int j,k;
	
	if (src_pID_ptr != kernel_ptr)
		j = src_pID_ptr->base_page;
	else
		j = kernel_base_page;
	
	while (page_ownersT[j].next_page != physical_page && page_ownersT[j].next_page != NULL_PAGE)
	{
		j = page_ownersT[j].next_page;
	}
	
	k = page_ownersT[page_ownersT[j].next_page].next_page;
	
	page_ownersT[j].next_page = k;
	
	if (src_pID_ptr != kernel_ptr) src_pID_ptr->size -= PAGE_SIZE;

	// add physical_page to dst page owner list:
	if (dst_pID_ptr != kernel_ptr)
		j = dst_pID_ptr->base_page;
	else
		j = kernel_base_page;
	
	while (page_ownersT[j].next_page != NULL_PAGE)
		j = page_ownersT[j].next_page;
	
	page_ownersT[j].next_page				= physical_page;
	page_ownersT[physical_page].next_page	= NULL_PAGE;
	
	if (!AddPageMec(dst_pID_ptr,physical_page,dst_virtual_address, permissions))
		return ERROR_GRANT_PAGE;
	
	dst_pID_ptr->size += PAGE_SIZE;
	
	return physical_page;
}

int32_t UnmapPage(process_table_t* pID_ptr, uint32_t virtual_address)
{
	int j;
	
	if (pID_ptr != kernel_ptr)
	{
		if (!CheckRange(pID_ptr, virtual_address)) return ERROR_ADDR_RANGE;
		j = pID_ptr->base_page;
	}
	else
		j = kernel_base_page;
	
	int32_t physical_page = RemovePageMec(pID_ptr, virtual_address);
	
	//Check if pID is page owner, and return to free list. NB don't use CheckPageOwner, because
	//we need the position in the page owner's list.
	while (page_ownersT[j].next_page != physical_page && page_ownersT[j].next_page != NULL_PAGE)
	{
		j = page_ownersT[j].next_page;
	}
	
	if (page_ownersT[j].next_page != NULL_PAGE)
	{
		/* Remove from process' page list, by going to page before physical_page, and 
		 * jumping over it.*/

		page_ownersT[j].next_page = page_ownersT[page_ownersT[j].next_page].next_page;
		
		//add to start of free list
		page_ownersT[physical_page].next_page = next_free_page;
		next_free_page = physical_page;
		num_free_pages++;
		
		RemoveMappings(physical_page);
	}
	else
	{
		/* Otherwise, remove it from the page map list and return to free list.*/
		
		// k is the target page being removed.
		int k;
		j = pID_ptr->page_maps;
		
		if (j == EMPTY_PAGE_MAP)
			return -1;
		
		else if (page_mapsT[j].physical_page == physical_page)
		{
			k = j;
			pID_ptr->page_maps = page_mapsT[k].next_pID;
		}
		else
		{
			k = page_mapsT[j].next_pID; // k is always a step ahead of j.
			
			while (page_mapsT[k].physical_page != physical_page)
			{
				j = page_mapsT[j].next_pID;
				k = page_mapsT[k].next_pID;
			}
			
			page_mapsT[j].next_pID = page_mapsT[k].next_pID;
		}
		
		// Skip over the map in physical page map list.
		// need to find entry before target k
		j = page_ownersT[physical_page].page_maps;
		
		if (j != k) // if first entry is not our target k:
		{
			while (page_mapsT[j].next_phys != k)
				j = page_mapsT[j].next_phys;
			
			page_mapsT[j].next_phys = page_mapsT[k].next_phys;
		}
		else // first entry is our target k
			page_ownersT[physical_page].page_maps = page_mapsT[k].next_phys;
		
		// return mapping to free list
		int old_free = next_free_map;
		next_free_map = k;
		page_mapsT[k].next_pID = old_free;
		page_mapsT[k].pID_ptr = 0;
		page_mapsT[k].physical_page = 0;
		page_mapsT[k].virtual_page = 0;
		page_mapsT[k].next_phys = EMPTY_PAGE_MAP;
	}
	
	pID_ptr->size -= PAGE_SIZE;
	
	return physical_page;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Memory Management Local Functions ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/* AppendNewPage() is really a local function, but it is used by the memory mechanisms.
 * It appends a new page to the address space starting at page j. Can pass any page as parameter
 * 'j' in the process' list  to get to the end, doesn't have to be first page in process' list. The
 * new page is appended at the start of the list.*/
int32_t AppendNewPage(int32_t j)
{
	if (j >= num_pages || j < 0)
		return ERROR_PHYS_PAGE_NUM;
	
	if (num_free_pages <= 0)
		return ERROR_NO_FREE_PAGES;
	
	int32_t k					= page_ownersT[j].next_page;
	page_ownersT[j].next_page	= next_free_page;
	next_free_page				= page_ownersT[next_free_page].next_page;
	
	page_ownersT[page_ownersT[j].next_page].next_page = k;
	num_free_pages--;
	
	return page_ownersT[j].next_page;
}

int RemoveMappings(uint32_t physical_page)
{
	// remove all mappings for a phys page and return to free list.
	// need to skip over links for process map lists.
	process_table_t* pID_ptr;

	int first_map	= page_ownersT[physical_page].page_maps;
	int next_map	= first_map;
	
	if (first_map != EMPTY_PAGE_MAP)
	{
		while (page_mapsT[next_map].next_phys != EMPTY_PAGE_MAP)
		{
			pID_ptr = page_mapsT[next_map].pID_ptr;
			
			RemovePageMec(pID_ptr, page_mapsT[next_map].virtual_page << PAGE_ADDRESS_SHIFT);
			
			pID_ptr->size						-= PAGE_SIZE;
			page_mapsT[next_map].physical_page	= 0;
			page_mapsT[next_map].virtual_page	= 0;
			page_mapsT[next_map].pID_ptr		= 0;
			
			/* link so list of phys maps can be appended to free list
			 * (which uses next_pID).*/
			page_mapsT[next_map].next_pID		= page_mapsT[next_map].next_phys;
			
			int j								= pID_ptr->page_maps;
			while (page_mapsT[j].physical_page != physical_page)
				j = page_mapsT[j].next_pID;
			
			page_mapsT[j].next_pID				= page_mapsT[next_map].next_pID;
			next_map							= page_mapsT[next_map].next_phys;
			page_mapsT[next_map].next_phys		= EMPTY_PAGE_MAP;
			num_free_maps++;
		}
		
		// append removed mappings to free list:
		int old_free_map				= next_free_map; // save old free value.
		next_free_map					= first_map;
		page_mapsT[next_map].next_pID	= old_free_map;
	}
	
	// if no mappings just return.
	return OK;
}

int32_t CheckPageOwner(process_table_t* pID_ptr, int32_t physical_page)
{
	// Check that current process is page owner, or coordinator
	if (current_pID == KERNEL_PID || pID_ptr == kernel_ptr)
		return OK;
	
	if ((pID_ptr->privilege & COORD_PRIVILEGE) != COORD_PRIVILEGE)
	{
		int j = pID_ptr->base_page;
		
		while (page_ownersT[j].next_page != physical_page
				&& page_ownersT[j].next_page != NULL_PAGE)
		{
			j = page_ownersT[j].next_page;
		}
		
		if (j == NULL_PAGE) //then current process is not page owner
			return 0;
	}
	
	// either current process is coordinator, or it is page owner.
	return OK;
}

int32_t CheckRange(process_table_t* pID_ptr, uint32_t virtual_address)
{
	if (pID_ptr != kernel_ptr)
		if (virtual_address >= USER_BOUNDARY)
			return 0;
	
	return OK;
}
// @P-

/* END OF FILE */
