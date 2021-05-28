///////////////////////////////////////////////////////////////////////////////////////////////////
/* Server
	excepter.c

	The server controls the whole system and is granted coordinator status by the kernel startup.
	It handles user requests, opens files from disk to then run them as processes, and handles page
	faults.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_development_server Server Code
// @A+

#include "server.h"
#include "../user/lib/syscall.h"
#include "../user/lib/stdlib.h"
#include "../kernel/mechanism/pc/kernel_ia32.h"
#include "../kernel/mechanism/pc/resources_pc.h"
#include "../kernel/mechanism/pc/boot/bootdata.h"
#include "../user/lib/debug.h"

#define NO_REPLY					0
#define SEND_REPLY					1
#define USER_READY_REPLY			2
#define SERVER_BUFFER_ADDRESS		0x6000

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global Data for Object Server //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/* The thread table stores current process IDs for the first thread in a process, and has a
 * bitfield for stack allocation. Bit 0 is used for the first thread, bit 20 for the 21st. The
 * kernel stacks are calculated from just below the kernel at 0x8000.0000 and each kernel stack is
 * 256 bytes. The user stacks are below the 32nd kernel stack at 0x8000.0000 -
 * 32 x 256 bytes = 0x7FFF.E000.*/

thread_table_t			threadT[NUM_THREAD_ENTRIES];
#define	KSTACK_0		(KERNEL_BASE - 4)
#define USTACK_0		(KERNEL_BASE - KSTACK_SIZE * NUM_THREAD_ENTRIES)
#define USTACK_SIZE		1024

typedef struct filetable_entry
{
	char	name[NAME_LENGTH];
	int32_t	block; //first block of object.
	int32_t	size;
} file_table;

boot_table_t	*g_bootT;
user_boot_t		*user_bootT;
file_table		*g_fileT;

uint32_t	request[SERVER_MSG_SIZE];
uint32_t	reply[SERVER_MSG_SIZE];
uint32_t	task_source;
uint32_t	max_num_procs;
int		video_ready;

pID_t	this_pID, request_pID;
pID_t	kernel_pID, idle_pID, default_excepter_pID, default_pager_pID;
pID_t	video_pID, keybd_pID, disk_pID;
pID_t	server_pID;
// the server value is required here simply because the stdlib.c defines it as extern.

///////////////////////////////////////////////////////////////////////////////////////////////////
// Object Server Interfaces ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int		Server(void);
int		DoRequest(pID_t pID);
int		NewProcess(void);
int		NewThread(void);

int		Open(void);
int		InitMemory(void);
int		WaitForUsers(int num_users);
int		InitUsers(void);

pID_t	OpenFile(char *filename, pID_t parent_pID, uint32_t priority);
int		CloseFile(pID_t pID);

int		LoadFile(char *filename, pID_t new_process, unsigned load_address);
int		LoadBlocks(pID_t pID, uint32_t load_address, block_array blocks, uint32_t permissions);

int32_t	FixPageFault(void);
int		PrintPageFault(void);
int		VideoReady(pID_t pID);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Server /////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int Server(void)
{
	int do_reply;
	
	Open();
	
	while (1)
	{
		request_pID	= Receive(ANY, request, SERVER_MSG_SIZE, 0);
		do_reply	= DoRequest(request_pID);
		
		if (do_reply != NO_REPLY)
		{
			Send(request_pID, reply, SERVER_MSG_SIZE, 0x1000);
			// This has a timeout in case the user does not behave properly by receiving the reply.
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// DoRequest - main message loop processing ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int DoRequest(pID_t request_pID)
{
	int action = NO_REPLY; // The default response for a user request is to not send a reply.
	
	switch (request[MSG_TYPE])
	{
		// System call requests ///////////////////////////////////////////////////////////////////
		case ADD_PAGE_SYSCALL :
		{
			/* int32_t AddPage(int32_t pID, uint32_t virtual_address, uint32_t permissions); */
			if (request[1] == USE_CURRENT_PROCESS)
				request[1] = request_pID;
			
			reply[0]	= AddPage(request[1], request[2], request[3]);
			action		= SEND_REPLY;
			break;
		}
		case MAP_PAGE_SYSCALL :
		{
			/* int32_t MapPage(int32_t src_pID, uint32_t src_virtual_address,
					int32_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions); */
			if (request[1] == USE_CURRENT_PROCESS)
				request[1] = request_pID;
			else if (request[3] == USE_CURRENT_PROCESS)
				request[3] = request_pID;
			
			reply[0]	= MapPage(request[1], request[2], request[3], request[4], request[5]);
			action		= SEND_REPLY;
			break;
		}
// @A-		
		case GRANT_PAGE_SYSCALL :
		{
			/* int32_t GrantPage(int32_t src_pID, uint32_t src_virtual_address,
					int32_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions); */
			if (request[1] == USE_CURRENT_PROCESS)
				request[1] = request_pID;
			else if (request[3] == USE_CURRENT_PROCESS)
				request[3] = request_pID;
			
			reply[0]	= GrantPage(request[1], request[2], request[3], request[4], request[5]);
			action		= SEND_REPLY;
			break;
		}
		case UNMAP_PAGE_SYSCALL :
		{
			/* int32_t UnmapPage(int32_t pID, uint32_t virtual_address); */
			if (request[1] == USE_CURRENT_PROCESS)
				request[1] = request_pID;
			
			reply[0]	= UnmapPage(request[1], request[2]);
			action		= SEND_REPLY;
			break;
		}
		case ADD_RESOURCE_SYSCALL :
		{
			/*int AddResource(int32_t pID, uint32_t resource_type, uint32_t index, uint32_t value);*/
			if (request[1] == USE_CURRENT_PROCESS)
				request[1] = request_pID;
			
			reply[0]	= AddResource(request[1], request[2], request[3], request[4]);
			action		= SEND_REPLY;
			break;
		}
		case REMOVE_RESOURCE_SYSCALL : // not coded yet.
		{
			reply[0] = -1;
			break;
		}
		case ADD_PROCESS_SYSCALL : // not sure this should be available at user level.
		{
			if (request[5] == USE_CURRENT_PROCESS)
				request[5] = request_pID;
			
			reply[0]	= AddProcess(request[1], request[5], 0, USER_STACK, KERNEL_STACK,
								  request[2], request[3]);
			
			//uint32_t AddProcess(uint32_t priority, int32_t parent, uint32_t instruction_pointer,
			//				   uint32_t user_stack, uint32_t kernel_stack, int32_t excepter, int32_t pager);
			
			action		= SEND_REPLY;
			break;
		}
// @A+
		// Server Requests ////////////////////////////////////////////////////////////////////////
		// Get Requests
		case MSG_GET :
		{
			switch (request[MSG_USER_TYPE])
			{
				case MSG_GET_COORDINATOR :
				{
					reply[0]	= this_pID;
					action		= SEND_REPLY;
					break;
				}
				case MSG_CHECK_PAGE :
				{
					// r1 is the virtual address, the return action is the page properties,
					// eg RWX, dirty, accessed.
					
					uint32_t properties[2];
					properties[0] = P_MEM_RIGHTS;
					int32_t result;
					
					if ((result = GetProcess(request_pID, properties)) < 0)
					{
						kprintf("Get error.");
						reply[0] = result;
					}
					else
					{
						reply[0] = properties[0];
					}
					
					kprintf("page mask ");
					printx(reply[0],8);
					action = SEND_REPLY;
					break;
				}
				default : break;
			}
			break;
		}
// @A-
		// Set Requests
		case MSG_SET :
		{
			switch (request[MSG_USER_TYPE])
			{
				case SET_PROCESS_SYSCALL :
				{
					reply[0] = SetProcess(request[2],request[3],request[4]);
								//uint32_t pID, uint32_t value, uint32_t reason);
					
					action = SEND_REPLY;
					break;
				}
				case MSG_SET_COORDINATOR : // involves SetProcess syscall.
				{
					if (request[5] == USE_CURRENT_PROCESS)
						request[5] = request_pID;
					
					// This rule determines who can and can't be a coordinator.
					if (request[5] > this_pID)
					{
						reply[0]	= -1;
						action		= SEND_REPLY;
						break;
					}
					
					reply[0]	= SetProcess(request[5],P_PRIVILEGE,COORD_PRIVILEGE);
					action		= SEND_REPLY;
					break;
				}
				case MSG_FILE_STATUS :
				{
					if (request[2] == USER_STATUS_INIT)
						action = USER_READY_REPLY;
					else
						action = NO_REPLY;
					
					break;
				}
				default : break;
			}
			break;
		}
// @A+
		// File & Process requests
		case MSG_OPEN_NAME:
		{
			action = NewProcess();
			break;
		}
		case MSG_NEW_THREAD:
		{
			action = NewThread();
			break;
		}
		case MSG_CLOSE_PID :
		{
			pID_t pID	= request[1];
			reply[0]	= CloseFile(pID);
			
			if (reply[0] == ERROR_PID_PTR)
			{
				kprintf("pID not found.");
			}
			else if (reply[0] == OK)
			{
				kprintf("Close ");
				printx(pID, 4);
			}
			
			action = SEND_REPLY;
			break;
		}
// @A-		
		// Startup info requests
		// For now, this returns the video driver pID
		case MSG_INFO :
		{
			reply[0]	= video_pID;
			action		= SEND_REPLY;
			break;
		}

		// Exceptions ////////////////////////////////////

		case EXCEPTION_SYSCALL:
		{
			if (request_pID > IDLE_PID)
			{
				if (FixPageFault()>0)
				{
					reply[0] = SetProcess(request_pID, P_READY, REASON_EXCEPTION_DONE);
				}
			}
			
			action = NO_REPLY;
			break;
		}
		// Default/error
		default :
		{
			return -1;
		}
	}
// @A+
    return action;
}

int NewProcess(void)
{
	int			action		= SEND_REPLY;
	pID_t		parent_pID	= request[1];
	uint32_t	priority	= request[2];
	priority				= priority & (~THREAD_FIELD);
	
	int8_t		filename[NAME_LENGTH];
	int8_t		*request_char = (int8_t*)&request[3];
	
	for (int i = 0; i < NAME_LENGTH; i++)
	{
		filename[i] = request_char[i];
	}
	
	pID_t new_pID = OpenFile(filename, parent_pID, priority);
	
	if (new_pID < 0)
	{
		if (new_pID == ERROR_FILE_OPEN)
			kprintf("error-file already open.");
		else
			kprintf("Error opening file.\n");
		
		action = -1;
	}
	else
	{
		kprintf("New process:");
		printx(new_pID,4);
		reply[1] = SetProcess(new_pID, P_READY, REASON_NEW);
	}
	
	reply[0] = new_pID;
	
	return action;
}

int NewThread(void)
{
	pID_t		parent_pID			= request[1]; // This is the existing process for the new thread.
	uint32_t	priority			= request[2];
	
	priority						= priority | (THREAD_FIELD); // force this to be a new thread.
	uint32_t	first_instruction	= 0;
	uint32_t	user_stack			= USER_STACK - PAGE_SIZE;
	uint32_t	kernel_stack		= KERNEL_STACK - KERNEL_STACK_SIZE;
	
	pID_t		new_thread_pID		= AddProcess(priority, parent_pID, first_instruction,
											 user_stack, kernel_stack,
											 default_excepter_pID, default_pager_pID);
	
	if (new_thread_pID < 0)
	{
		kprintf("Error creating thread ");
		printx(new_thread_pID, 2);
		reply[0] = new_thread_pID;
	}
	else
	{
		kprintf("New thread:");
		printx(new_thread_pID, 4);
		reply[0] = SetProcess(new_thread_pID, P_READY, REASON_NEW);
	}
	
	return SEND_REPLY;
}
// @A-

///// Initialisation Functions ////////////////////////////////////////////////////////////////////

int Open(void)
{
	SetProcess(USE_CURRENT_PROCESS, P_PRIVILEGE, COORD_PRIVILEGE);

	// Initialise the thread table.
	for (int i = 0; i < NUM_THREAD_ENTRIES; i++)
	{
		threadT[i].pID = 0;
		threadT[i].stack_bitmap = 0;
		threadT[i].num_faults = 0;
	}
	
	uint32_t properties[2];
	properties[0]			= P_PID;
	GetProcess(USE_CURRENT_PROCESS, properties);
	this_pID				= properties[0];
	
	properties[0]			= K_MAX_PROCESSES;
	GetProcess(USE_CURRENT_PROCESS, properties);
	max_num_procs			= properties[0];
	
	kernel_pID				= KERNEL_PID;
	idle_pID				= IDLE_PID;
	disk_pID				= 0x2001;
	video_pID				= keybd_pID = 0;
	default_excepter_pID	= default_pager_pID = this_pID;
	
	video_ready				= 0;

	InitMemory();
	
	WaitForUsers(1);
	/* This should be set to number of boot objects the server would wait on. This doesn't include
	 * itself and Idle. Wait for disk driver to report it is ready for requests, otherwise halt.*/
	
	InitUsers();

	return 1;
}

int InitMemory(void)
{
	uint32_t src_virtual_address, dst_virtual_address, i;
	
	for (i = 0; i < NUM_FS_LOWMEM_PAGES; i++)
	{
		src_virtual_address = KERNEL_LOWMEM + i*PAGE_SIZE;
		dst_virtual_address = FS_LOWMEM + i*PAGE_SIZE;
		MapPage(KERNEL_PID, src_virtual_address, USE_CURRENT_PROCESS,
				dst_virtual_address, USER_RWX);
	}

	// Init the location of the file table.
    g_bootT					= (boot_table_t*)(FS_BOOT_TABLE);
    boot_table_t *boot_device	= g_bootT;
    boot_device				+= 4;
	
	unsigned ft				= boot_device->block * BLOCK_SIZE + FS_BLOCK_BASE;
	g_fileT					= (file_table*)ft;
	
	return 1;
}

int WaitForUsers(int num_users)
{
	int do_reply;
	
	/* This loop has to be able to process requests other than just waiting for the user to be
	 * ready.*/
	while (num_users)
	{
		request_pID	= Receive(ANY, request, SERVER_MSG_SIZE, 0);
		do_reply	= DoRequest(request_pID);
		
		if (do_reply == USER_READY_REPLY)
		{
			num_users--;
		}
		else if (do_reply == SEND_REPLY)
		{
			Send(request_pID, reply, SERVER_MSG_SIZE, 0);
		}
	}

	return 1;
}

int InitUsers(void)
{
	default_excepter_pID	= OpenFile("excepter.bin",	this_pID, FAST_PRIORITY);
	video_pID				= OpenFile("video.bin",		this_pID, FAST_PRIORITY);
	keybd_pID				= OpenFile("keybd.bin",		this_pID, FAST_PRIORITY);

	SetProcess(default_excepter_pID,	P_READY, REASON_NEW);
	SetProcess(video_pID,				P_READY, REASON_NEW);
	
	/* Wait for the video to become ready before opening any more files. This also means that no
	 * other user files can be opened yet.*/
	WaitForUsers(1);
	
	SetProcess(keybd_pID,				P_READY, REASON_NEW);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Object Table Functions //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Open/Close Object //////////////////////////////////////////////////////////////////////////////

pID_t OpenFile(char *filename, pID_t parent_pID, uint32_t priority)
{
	int			add_thread			= 0;
	uint32_t	instruction_pointer	= 0;
	uint32_t	ustack				= USER_STACK;
	uint32_t	kstack				= KERNEL_STACK;
	uint32_t	user_memory_start	= 0;

	if ((priority & THREAD_FIELD) == THREAD_FIELD)
	{
		add_thread++;
	}

	/* pID_t AddProcess(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
	 *					uint32_t user_stack_addr, uint32_t kernel_stack_addr,
	 *					pID_t excepter_pID, pID_t pager_pID);*/
	pID_t new_pID = AddProcess(priority, parent_pID, instruction_pointer, ustack, kstack,
								default_excepter_pID, default_pager_pID);
	
	if (new_pID < MIN_PID)
		return new_pID;

	if (new_pID >= 0 && add_thread == 0) // load from storage/object system
	{
		// load process or library from storage
		int pages_granted = LoadFile(filename, new_pID, user_memory_start);

		if (pages_granted < 1)
			return pages_granted;
	}
	
	return new_pID;
}

int CloseFile(pID_t pID)
{
	return RemoveProcess(pID);
}

// Load/Save Object ///////////////////////////////////////////////////////////////////////////////

int LoadFile(char *filename, pID_t new_pID, unsigned load_address)
{
	/* 1. Search for the file.
	 * Start at the start of the file table.
	 * The file table is 2 blocks, each entry is 32 bytes, so that's 64 files.
	 * There is no linking, just a flat file space.*/
	
	file_table *ft	= g_fileT;
	int			i	= 0;
	int			max	= 2 * BLOCK_SIZE / (sizeof(file_table));
	
	while (i < max)
	{
		if (kstrcmp(filename, ft->name) == 0)
			break;
		
		ft++;
		i++;
	}
	
	if (i == max)
		return -1; // file not found.
	
	// 2. Load the blocks, one page at a time. DO over larger loop on file size.
	// The blocks are contiguous, so we can just issue a request in order. If the file size has been reached,
	// then some of the blocks array can have 0's.
	block_array	blocks;
	block_t		current_block	= ft->block;
	int32_t		file_size		= ft->size;
	block_t		last_block		= current_block + (block_t)(file_size);
	uint32_t	permissions		= USER_RWX;
	int			pages_granted	= 0;
	int			done			= 0;
	
	while (!done)
	{
		for (int i = 0; i < BLOCKS_PER_PAGE; i++)
		{
			blocks[i] = 0;
			
			if (current_block < last_block)
				blocks[i]	= current_block++;
			else
				done		= 1;
		}
		
		pages_granted	+= LoadBlocks(new_pID, load_address, blocks, permissions);
		load_address	+= PAGE_SIZE;
	}
	
	return pages_granted;
}

// Load Blocks ////////////////////////////////////////////////////////////////////////////////////

int LoadBlocks(pID_t user_pID, uint32_t load_address, block_array blocks, uint32_t permissions)
{
	unsigned buffer_address = SERVER_BUFFER_ADDRESS;

	AddPage(disk_pID, buffer_address, USER_RWX);
	
	int i = 0;

	// Skip any 0 values at the start of the array.
	while (blocks[i] == 0 && i < BLOCKS_PER_PAGE)
		i++;

	// Start from the first non-zero value in the blocks array.
	for(; i < BLOCKS_PER_PAGE; i++)
	{
		buffer_address = SERVER_BUFFER_ADDRESS + BLOCK_SIZE*i;
		
		// The MSG_BLOCK_READ reads just one block at a time.
		if (blocks[i] != 0)
		{
			request[MSG_TYPE]	= MSG_BLOCK_READ;
			request[1]			= blocks[i];
			request[2]			= buffer_address;
			Send(disk_pID, request, SERVER_MSG_SIZE, 0);
			Receive(disk_pID, request, SERVER_MSG_SIZE, 0);
		}
	}

	GrantPage(disk_pID, SERVER_BUFFER_ADDRESS, user_pID, load_address, permissions);

	return 1; // number of pages!!
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Page Fault //////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int32_t FixPageFault(void)
{
	int i = 0;
	
	while (threadT[i].pID != request_pID && i < NUM_THREAD_ENTRIES)
		i++;
	
	if (i >= NUM_THREAD_ENTRIES)
		return -1;
	
	if (threadT[i].num_faults > 4)
	{
		SetProcess(request_pID, P_BLOCKED, REASON_EXCEPTION);
		return -1;
	}
	
	threadT[i].num_faults++;

	return AddPage(request_pID, request[CR2], USER_RWX);
}

int PrintPageFault(void)
{
// Buffer is: EXC_NO|CR2|error code|EIP|CS|EFLAGS|ESP|SS
// error code values:
// error code	User/Kernel		Read/Write		Present
// 0			kernel			read			page not present
// 1			kernel			read			present
// 2			kernel			write			page not present
// 3			kernel			write			present
// 4			user			read			page not present
// 5			user			read			present
// 6			user			write			page not present
// 7			user			write			present
// MSG_TYPE | EXC_NO | CR2 | ERROR_CODE | EIP | CS | EFLAGS | ESP | SS
	
	int present			= request[ERROR_CODE] & 0x1;
	int read_write		= request[ERROR_CODE] & 0x2;
	int user_or_kernel	= request[ERROR_CODE] & 0x4;
	int reserved		= request[ERROR_CODE] & 0x8;

	kprintf("pID ");
	printx(request_pID, 2);
	kprintf(" CR2 ");
	printx(request[CR2], 8);
	
	present				? kprintf(" P|")	: kprintf(" NP|");
	read_write			? kprintf("W|")		: kprintf("R|");
	user_or_kernel		? kprintf("U ")		: kprintf("K ");
	
	if (reserved)
		kprintf("Res ");

	kprintf("eip ");
	printx(request[EIP], 8);
	kprintf(" eflags ");
	printx(request[EFLAGS], 8);

	request[CS] == 0x08	? kprintf(" Ck")	: kprintf(" Cu");
	
	return 1;
}

int VideoReady(pID_t pID)
{
	request[MSG_TYPE] = MSG_PRINT_NAME;
	Send(pID, request, SERVER_MSG_SIZE, 100);
	video_ready++;
	return 1;
}

/* END OF FILE */
