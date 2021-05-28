///////////////////////////////////////////////////////////////////////////////////////////////////
/* Process Table Display - ptd
	processtable.c

	This program is used to convert the memory dump of the kernel process table in bochs, into a
	more meaningful text display.
	
	It can also be used to convert 'reason' codes into 'reason' text or error text.
	
	Refer to the PrintHelp function, or run the program without any options to see more on what
	this program does.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../../kernel/policy/global.h"
#include "../colors.h"

//// Reason Strings
// Reasons for blocking, and waking up.
char reason_strings[][30] = 
{"pending", "new", "removed", "running", "running done", "blocked", "--", "--",  "--",  "--",  "--",  "--",  "--",  "--",  "check failed",  "--", "send", "send done", "send timeout",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--", 
"receive", "receive done", "receiver timeout",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--",  "--", "message target removed",
"interrupt", "interrupt done", "exception", "exception done"
};
#define REASON_OFFSET REASON_PENDING
#define REASON_LAST REASON_EXCEPTION_DONE

char syscall_strings[][3] = 
{"SH", "AP", "RP", "GP", "SP", "Sx", "Rx", "AR", "RR", "In", "Ex", "Ap", "Mp", "Gp", "Up"};

///// Error Codes ///////////////////////////////////////////////////////////////////////
char error_strings[][40] = 
{"error general", "error process table full","error add process","error check add process","error add memory","error add process mec","error check failed","error get process","error pid ptr","error get process property","error set process","error set process property","error set blocked","error no coordinator","error check add resource","error int num range","error port num range","error add resource","error remove resource check","error remove resource","error remove process resources check","error read","error write","error send failed","error receive failed","error deadlock","error send failed no target","error receive failed no target","error no int handler","error not interrupt handler","error no exc handler","error no free pages","error no physical page","error page map process not found","error grant page src pid","error src not page owner","error grant page dst pid","error grant unmap not owner","error check page owner","error add kernel page","error page maps full","error phys page num","error addr range","error grant page","error page already mapped","error map num","error no free maps"
};

#define ERROR_OFFSET	ERROR
#define ERROR_LAST		ERROR_KERNEL_LAST

FILE *g_file_in_ptr; //temp pointer.
FILE *g_file_out_ptr; // for new file.
FILE *g_file_log_ptr; // log of output is created in log_name

int option_all = 0; // display: d, b, m, p, s, x
int option_d = 0; // display on stdout
int option_f = 0; // display on logfile (inputfile.ptd)
int option_b = 0; // display message buffers
int option_m = 0; // display message queues
int option_p = 0; // display process table
int option_s = 0; // display schedule queues
int option_x = 0; // display mechanisms
int option_i = 0; // input format, if specified then uses text input.
int option_debug = 0;
int number_processes;

#define PROCESS_TABLE_ENTRY_SIZE 17*4
int pt[MAX_PROCESSES][PROCESS_TABLE_ENTRY_SIZE]; // 16 process table entries, 16 rows of 4 ints per pt entry.
process_table_t* pt_ptr, *first_pt_ptr, *last_pt_ptr;

void	dprint(const char *fmt, ...);
int32_t	Convert_ptr_pID(process_table_t* pID_ptr);

process_table_t* Convert_kptr_ptr(process_table_t* kptr);

int		InterpretError(char *error_str);
int		PrintTable(void);
int		PrintIntro(void);
int		PrintReason(process_table_t* pt_ptr);
int		PrintFamily(process_table_t* pt_ptr);
int		PrintMechanisms(void);
int		MessageQueues(void);
int		MessageBuffers(void);
int		ScheduleQueues(void);
int		OpenOutfile(char *outfile);
int		ConvertTextInfile(char *infile);
int		ConvertBinaryInfile(char *infile);
int		PrintHelp(int no_args);
int		ConvertOptions(char *options, char *error_code);

int main(int argc, char* argv[])
{
	PrintHelp(argc);
	ConvertOptions(argv[1], argv[2]);
	
	if (option_f == 1)
		OpenOutfile(argv[2]);
	
	if (option_i == 1)
		ConvertTextInfile(argv[2]);
	else
		ConvertBinaryInfile(argv[2]);
	
	PrintIntro();
	
	if (option_p==1)	PrintTable();
	if (option_s==1)	ScheduleQueues();
	if (option_m == 1)	MessageQueues();
	if (option_b == 1)	MessageBuffers();
	if (option_x == 1)	PrintMechanisms();
	
	fclose(g_file_in_ptr);
	fclose(g_file_out_ptr);
	
	return 0;
}

/* dprint takes control of the usual printf according to the command-line options.
 * If -f is specified, then the output is redirected to the output file.
 * If -d is specified, then the output is directed to stdout.
 * Both of these options can be used at once.
 */
void dprint(const char *fmt, ...)
{
	va_list argp;
	
	if (option_f == 1)
	{
		va_start(argp, fmt);
		vfprintf(g_file_out_ptr, fmt, argp);
		va_end(argp);
	}
	if (option_d == 1)
	{
		va_start(argp, fmt);
		vprintf(fmt, argp);
		va_end(argp);
	}
}
	
int PrintHelp(int no_args)
{
	if (no_args != 3)
	{
		printf("!!ptd: process table display tool requires 2 arguments (%d used):\n", no_args-1);
		printf("ptd -options inputfile.\n");
		printf("\tThe 'inputfile' should be a plain text file of the captured memory dump in bochs of the process table.\n\tThe output is written to inputfile.ptd.\n");
		printf
		(
			"\tThe options are:\n\
			i - the input file is a text file, if not specified then it's a binary file.\n\
			d - display in stdout.\n\
			f - display in logfile (inputfile.ptd)\n\
			a - select all of the following options (except 'e')\n\
			m - produce message queues.\n\
			b - produce message buffers.\n\
			s - produce schedule queues.\n\
			p - produce process table summary.\n\
			x - produce process mechanisms.\n\
			e - convert an error/reason code from hex to its text meaning,\n\
				 eg ptd -e FFFFFFDD, or ptd -e -23, or ptd -e -0x23\n\
			To disable one of the options, use the capital letter equivalent.\
			eg ptd -daX inputfile >> this will enable all options first, then disable 'x'.\
		");
		exit(-3);
	}
	return 1;
}

int ConvertOptions(char *options, char *error_code)
{
	if (options[0] != '-')
		exit(-4);
	else
	{
		int i=0;
		while (options[i] != 0)
		{
			switch (options[i])
			{
				case 'a' : option_all = option_d = option_b = option_m = option_p = option_s = option_x = 1; break;
				case 'd' : option_d = 1; break;
				case 'f' : option_f = 1; break;
				case 'm' : option_m = 1; break;
				case 'M' : option_m = 0; break;
				case 'p' : option_p = 1; break;
				case 'P' : option_p = 0; break;
				case 'b' : option_b = 1; break;
				case 'B' : option_b = 0; break;
				case 's' : option_s = 1; break;
				case 'S' : option_s = 0; break;
				case 'x' : option_x = 1; break;
				case 'X' : option_x = 0; break;
				case 'i' : option_i = 1; break;
				case 'e' : InterpretError(error_code); exit('e');
			}
			i++;
		}
	}
	return 1;
}

int OpenOutfile(char *outfile)
{
	char file_out_str[80];
	strcpy (file_out_str, outfile);
	strcat(file_out_str, ".ptd");
	g_file_out_ptr = fopen(file_out_str, "w");
	if (g_file_out_ptr == NULL)
	{
		printf("open %s failed\n",outfile);
		exit(-2);
	}
	return 1;
}
	
int ConvertTextInfile(char *infile)
{
	char s1[20], s2[20];
	long temp;
	int *ptaddr;
	ptaddr = (int*)&pt[0][0];
	int	x1 = 0;
	temp = 0;
	g_file_in_ptr = fopen(infile, "r"); 
	
	if (g_file_in_ptr == NULL)
	{
		printf("file %s open failed\n", infile);
		fclose(g_file_in_ptr);
		exit(-3);
	}
	
	int i = 0;
	while (!feof(g_file_in_ptr))
	{
		fscanf(g_file_in_ptr, "%X%s%s%X%X%X%X\n", &x1, s1, s2, &ptaddr[i], &ptaddr[i+1], &ptaddr[i+2], &ptaddr[i+3]);
		i+=4;
	}
	number_processes = i/64;
	first_pt_ptr = (process_table_t*)&pt[0][0];
	last_pt_ptr  = (process_table_t*)&pt[number_processes - 1][0];
	if (number_processes != first_pt_ptr->msg[2])
	{
		printf("\nError: number of processes read from input file (%d) don't match current number of processes in running kernel (%lu).\n", number_processes, first_pt_ptr->msg[2]);
		exit(-6);
	}
	return 1;
}

int ConvertBinaryInfile(char *infile)
{
	g_file_in_ptr = fopen(infile, "rb");
	
	if (g_file_in_ptr == NULL)
	{
		fputs("File error",stderr);
		exit(1);
	}

	// obtain file size:
	fseek(g_file_in_ptr, 0, SEEK_END);
	long file_size = ftell(g_file_in_ptr);
	rewind(g_file_in_ptr);
	int *ptaddr;
	
	long i = 0;
	int j = 0;
	
	while (i < file_size)
	{
		ptaddr = (int*)&pt[j][0];
		fread(ptaddr, 4, 68, g_file_in_ptr);
		i += (4*68);
		j++;
	}
	number_processes = j;
	first_pt_ptr = (process_table_t*)&pt[0][0];
	last_pt_ptr  = (process_table_t*)&pt[number_processes - 1][0];
	if (number_processes != first_pt_ptr->msg[2])
	{
		printf("\nError: number of processes read from input file (%d) don't match current number of processes in running kernel (%lu).\n", number_processes, first_pt_ptr->msg[2]);
		exit(-6);
	}
	return 1;
}
	
int PrintIntro(void)
{
	dprint(BLUE "\n\t\tProcess Table Display tool - ptd - (c) Paul Cuttler 2013 (v1.1)\n\n" NORMAL);
	return 1;
}

int PrintTable(void)
{
	pt_ptr = first_pt_ptr;
	dprint(WHITE_UNDERLINE "\npID   state  targ  sc TSS BPg timer ");
	if (option_x == 1) dprint("usrstack knlstack ");
	dprint("parent reason   family...\n" NORMAL);
	while (pt_ptr <= last_pt_ptr)
	{
		if (pt_ptr->status != 0)
		{
			char privilege = '-';
			if (pt_ptr->privilege == 1) privilege = '*';
			else if (pt_ptr->privilege == 2) privilege = '+';
			else if (pt_ptr->privilege == 3) privilege = '&';
			int syscall = pt_ptr->last_syscall - FIRST_SYSCALL;
			if (syscall < 0) syscall = LAST_SYSCALL - FIRST_SYSCALL + 1;
			if (pt_ptr->status == READY) dprint(YELLOW);
			else if (pt_ptr->status == RUNNING) dprint(RED);
			dprint("%04x%c %01x %02x %01x " NORMAL,pt_ptr->pID, privilege, pt_ptr->status, pt_ptr->times_scheduled, pt_ptr->priority);
			dprint("%05x %2s ", pt_ptr->prev_msg_target_pID, syscall_strings[syscall]);
			dprint("%03x %03x %05x ", pt_ptr->mechanisms[1], pt_ptr->base_page, pt_ptr->timer & 0xFFFFF);
			if (option_x == 1)
			{
				dprint("%08x %08x ", pt_ptr->user_stack_addr, pt_ptr->kernel_stack_addr);
			}
			dprint("%04x ",pt_ptr->parent_pID);
			PrintReason(pt_ptr);
			if (pt_ptr->child != 0)
				PrintFamily(pt_ptr);
			dprint("\n");
		}
		pt_ptr++;
	}
	dprint("(Next to the pID: & = coordinator and address space (AS) owner, + = AS owner, * = coordinator, - = thread.)\n");
	// print other kernel data from spare Idle values:
	dprint("\ncurrent pID: %04x ", first_pt_ptr->msg[0]);
	dprint(YELLOW "num_schedules: %04x ", first_pt_ptr->msg[1]);
	dprint(NORMAL "num_processes: %04x (word count for bochs %d) ", first_pt_ptr->msg[2], first_pt_ptr->msg[2]*PROCESS_TABLE_ENTRY_SIZE);
	dprint(YELLOW "num_sleepers: %04x\n", first_pt_ptr->msg[3]);
	process_table_t* readyqueue = Convert_kptr_ptr((process_table_t*)first_pt_ptr->msg[4]);
	int32_t readypID = 0;
	if (readyqueue != 0) readypID = readyqueue->pID;
	process_table_t* fastqueue = Convert_kptr_ptr((process_table_t*)first_pt_ptr->msg[5]);
	int32_t fastpID = 0;
	if (fastqueue != 0) fastpID = fastqueue->pID;
	process_table_t* blockedqueue = Convert_kptr_ptr((process_table_t*)first_pt_ptr->msg[6]);
	int32_t blockedpID = 0;
	if (blockedqueue != 0) blockedpID = blockedqueue->pID;
	process_table_t* runningpointer = Convert_kptr_ptr((process_table_t*)first_pt_ptr->msg[7]);
	int32_t runningpID = 0;
	if (runningpointer != 0) runningpID = runningpointer->pID;

	dprint(YELLOW "readyQ: %04x ", readypID);
	dprint(NORMAL "ready_fastQ: %04x ", fastpID);
	dprint(YELLOW "blockedQ: %04x ", blockedpID);
	dprint(NORMAL "running_ptr: %04x \n", runningpID);
	return 1;
}

int MessageQueues(void)
{
	/*	uint32_t msg[MSG_MAX_LENGTH];
	int32_t msg_range_length;
	uint32_t timer;
	*/
	process_table_t* temp_pt_ptr, *msgQ_ptr;
	pt_ptr = first_pt_ptr;
	int32_t temp_pID = 0;
	dprint(RED_UNDERLINE "\nMessage Queues:\n" NORMAL);
	while (pt_ptr <= last_pt_ptr)
	{
		msgQ_ptr = Convert_kptr_ptr(pt_ptr->own_msgQn);
		if (pt_ptr != msgQ_ptr && msgQ_ptr != 0) // originally message empty message queues were indicated by 0, now by 'self' pointer. This line makes ptd tool compatible with old and new dump files.
		{
			char reason = '?';
			if (pt_ptr->reason == REASON_SEND) reason = 'S';
			else if (pt_ptr->reason == REASON_RECEIVE) reason = 'R';
			dprint("pID %04x[%02x%c] = ",pt_ptr->pID, pt_ptr->msg_range_length, reason);
			if (msgQ_ptr->reason == REASON_SEND) reason = 'S';
			else if (msgQ_ptr->reason == REASON_RECEIVE) reason = 'R';
			else reason = '?';
			dprint("%04x[%02x%c]",msgQ_ptr->pID, msgQ_ptr->msg_range_length, reason);
			msgQ_ptr = Convert_kptr_ptr(msgQ_ptr->wait_msgQn);
			while(msgQ_ptr->wait_msgQn !=0 && msgQ_ptr != pt_ptr)
			{
				if (msgQ_ptr->reason == REASON_SEND) reason = 'S';
				else if (msgQ_ptr->reason == REASON_RECEIVE) reason = 'R';
				dprint("->%04x[%02x%c]",msgQ_ptr->pID, msgQ_ptr->msg_range_length, reason);
				msgQ_ptr = Convert_kptr_ptr(msgQ_ptr->wait_msgQn);
			}
			dprint("->%04x.\n",msgQ_ptr->pID);
		}
		pt_ptr++;
	}
	pt_ptr = first_pt_ptr;
	dprint(RED "Interrupts:\n" NORMAL);
	while (pt_ptr <= last_pt_ptr)
	{
		int int_num = (int)pt_ptr->wait_msgQp;
		if (int_num >= HWINT_FIRST && int_num <= HWINT_LAST)
		{
			int_num = int_num & 0xFF;
			dprint("int %02x = %04x[%02x]",int_num, pt_ptr->pID, pt_ptr->msg_range_length);
			msgQ_ptr = Convert_kptr_ptr(pt_ptr->wait_msgQn);
			//dprint("msgQ_ptr %08x\n",msgQ_ptr);
			while(msgQ_ptr != NULL && msgQ_ptr->wait_msgQn != NULL && msgQ_ptr != (process_table_t*)int_num)
			{
				//if (msgQ_ptr->reason == REASON_SEND) reason = 'S';
				//else if (msgQ_ptr->reason == REASON_RECEIVE) reason = 'R';
				dprint("->%04x[%02x]",msgQ_ptr->pID, msgQ_ptr->msg_range_length);
				msgQ_ptr = Convert_kptr_ptr(msgQ_ptr->wait_msgQn);
			}
			if (msgQ_ptr != NULL) dprint("->%x",msgQ_ptr->pID);
			dprint(".\n");
		}
		pt_ptr++;
	}
	dprint(NORMAL);
	return 1;
}

int MessageBuffers(void)
{
	//////////////// Message Buffers ///////////////////////////////////
	process_table_t* temp_pt_ptr, *Pn_ptr;
	pt_ptr = first_pt_ptr;
	int32_t temp_pID = 0;
	dprint(RED_UNDERLINE "\nMessage Buffers (note: hex values and characters print in opposite endian order).\n" NORMAL);
	pt_ptr = first_pt_ptr;
	while (pt_ptr <= last_pt_ptr)
	{
		if (pt_ptr->pID != 0)
		{
			dprint("%04x[%01x]:", pt_ptr->pID, pt_ptr->msg_range_length & 0x0F);
			int i;
			for (i=0; i < MSG_MAX_LENGTH; i++) dprint("%08x ", pt_ptr->msg[i]);
			char *msg_char = (char*)pt_ptr->msg;
			dprint("\n%04x:   ", pt_ptr->pID);
			for (i=0; i < MSG_MAX_LENGTH*4; i++)
			{
				if (msg_char[i] >= 0x20 && msg_char[i] <= 0x7e)	dprint("%c ", msg_char[i]);
				// this only prints sensible ascii characters.
				else dprint("  ");
				if (i % 4 == 3) dprint(" ");
			}
			dprint("\n");
		}
		pt_ptr++;
	}
	return 1;
}

int32_t Convert_ptr_pID(process_table_t* pID_ptr)
{
	uint32_t index = (uint32_t)pID_ptr;
	index = (index & 0xF00) >> 8;
	int32_t temp_pID = pt[index][0];
	printf("convert pID ptr %08x:: index=0x%X, pID=%08x\n",pID_ptr, index, temp_pID);
	return temp_pID;
}

process_table_t* Convert_kptr_ptr(process_table_t* kptr)
{
	if (kptr == NULL) return NULL;
	int int_num = (int)kptr;
	if (int_num >= HWINT_FIRST && int_num <= HWINT_LAST) return NULL;
	process_table_t* pt_ptr;
	uint32_t index = (uint32_t)kptr;
	index = (index & 0xF00) >> 8;
	pt_ptr = (process_table_t*)&pt[index][0];
	if (option_debug == 1) printf("\n\nconvert pID ptr %08x:: index=0x%X, pt_ptr=%08x\n\n",kptr, index, pt_ptr);
	return pt_ptr;
}
	
int ScheduleQueues(void)
{
	process_table_t* start_ptr;
	int i=0;
	pt_ptr = first_pt_ptr;
	// find first blocked process
	while (pt_ptr->status != BLOCKED && pt_ptr <= last_pt_ptr)
		pt_ptr++;
	if (pt_ptr <= last_pt_ptr) //make sure the queue isn't empty.
	{
		dprint("Blocked Queue: %04x",pt_ptr->pID);
		// collect the rest of the blocked processes as linked list.
		start_ptr = pt_ptr;
		pt_ptr = Convert_kptr_ptr(pt_ptr->next);
		i=0;
		while (pt_ptr != start_ptr && i<MAX_PROCESSES)
		{
			if (pt_ptr->status == BLOCKED) dprint("->%04x",pt_ptr->pID);
			pt_ptr = Convert_kptr_ptr(pt_ptr->next);
			i++;
		}
		dprint("->%04x.\n",start_ptr->pID);
	}
	////////// Ready Queue /////////////////////////////
	// if priority is < 8, goes in normal queue, otherwise in fast queue.
	pt_ptr = first_pt_ptr;
	pt_ptr++; // skip Idle because it isn't in a queue.
	// find first ready process
	while (
		((pt_ptr->status != READY && pt_ptr->status != RUNNING)
		 || pt_ptr->priority >= 8)
		  && pt_ptr <= last_pt_ptr)
	{
		pt_ptr++;
	}
	if (pt_ptr <= last_pt_ptr)
	{
		dprint("Ready queue: %04x",pt_ptr->pID);
		// collect the rest of the blocked processes as linked list.
		//option_debug = 1;
		start_ptr = pt_ptr;
		pt_ptr = Convert_kptr_ptr(start_ptr->next);
		//option_debug = 0;
		//dprint("::: start->next %08x, pt_ptr %08x (%04x), start %08x (%04x)\n", start_ptr->next, pt_ptr, pt_ptr->pID, start_ptr, start_ptr->pID);
		i=0;
		//dprint("## start ptr %08x (%04x), pt_ptr %08x (%04x)##",start_ptr, start_ptr->pID, pt_ptr, pt_ptr->pID);
		while (pt_ptr != start_ptr && i<MAX_PROCESSES)
		{
			if (pt_ptr->status == READY || pt_ptr->status == RUNNING || pt_ptr->priority < 8) dprint("->%04x",pt_ptr->pID);
			pt_ptr = Convert_kptr_ptr(pt_ptr->next);
			i++;
		}
		dprint("->%04x.\n",start_ptr->pID);
	}
	else
		dprint("No processes in ready queue.\n");
	
	////////// Fast Ready Queue /////////////////////////////
	// if priority is >= 8, goes in normal queue, otherwise in fast queue.
	pt_ptr = first_pt_ptr;
	pt_ptr++; // skip Idle because it isn't in a queue.
	// find first fast process
	while (
		//(pt_ptr->status != READY && pt_ptr->status != RUNNING)
		(pt_ptr->status == BLOCKED || pt_ptr->priority < 8)
		&& pt_ptr <= last_pt_ptr)
	// status == READY or == RUNNING and priority >= 8
	{
		pt_ptr++;
	}
	if (pt_ptr <= last_pt_ptr)
	{
		dprint("Fast ready queue: %04x",pt_ptr->pID);
		// collect the rest of the blocked processes as linked list.
		start_ptr = pt_ptr;
		pt_ptr = Convert_kptr_ptr(start_ptr->next);
		i=0;
		while (pt_ptr != start_ptr && i<MAX_PROCESSES)
		{
			if (pt_ptr->status == READY || pt_ptr->status == RUNNING || pt_ptr->priority >= 8) dprint("->%04x",pt_ptr->pID);
			pt_ptr = Convert_kptr_ptr(pt_ptr->next);
			i++;
		}
		dprint("->%04x.\n",start_ptr->pID);
	}
	else
		dprint("No processes in fast ready queue.\n");
	return 1;
}

int PrintReason(process_table_t* pt_ptr)
{
	int8_t reason = pt_ptr->reason;
	char sign = ' ';
	char rstring[50];
	if (reason < 0)
	{
		reason = 0 - reason;
		sign = '-';
		strcpy(rstring, error_strings[reason + ERROR_OFFSET]);
	}
	else
		strcpy(rstring, reason_strings[reason - REASON_OFFSET]);
	dprint("%c%02x %s ", sign, reason, rstring);
	return 1;
}

int PrintFamily(process_table_t* pt_ptr)
{
	process_table_t* temp_pt_ptr, *sibling_ptr;
	int i = 1;
	sibling_ptr = Convert_kptr_ptr(pt_ptr->sibling);
	int32_t sibling_pID = 0;
	if (sibling_ptr != 0) sibling_pID = sibling_ptr->pID;
	//dprint("\t0x%X's parent: 0x%X, ", pt_ptr->pID, pt_ptr->parent_pID);
	dprint("**Family: sibling: 0x%X ", sibling_pID);
	if (pt_ptr->child != 0)
	{
		temp_pt_ptr = Convert_kptr_ptr(pt_ptr->child);
		while (temp_pt_ptr != 0 && i<10)
		{
			dprint("c%d %X ", i, temp_pt_ptr->pID);
			i++;
			temp_pt_ptr = Convert_kptr_ptr(temp_pt_ptr->sibling);
		}
	}
	return 1;
}

int PrintMechanisms(void)
{
	process_table_t* pt_ptr = first_pt_ptr;
	uint32_t esp0 = 0, eip = 0, eflags = 0, eax = 0, ebx = 0, ecx = 0, edx = 0, esp = 0, ebp = 0, esi = 0, edi = 0;
	dprint(GREEN_UNDERLINE "\nProcess Mechanisms (if values aren't shown it's because they are the same as pID 0x1001).\n" NORMAL);
	while (pt_ptr <= last_pt_ptr)
	{
		if (pt_ptr->pID != 0)
		{
			//dprint("tss sel: %04x ", pt_ptr->mechanisms[0]);
			dprint("pID %04x|", pt_ptr->pID);
			if (pt_ptr->pID == 0x1001)
			{
				dprint("(set defaults here)");
				esp0 = pt_ptr->mechanisms[3];
				eip =  pt_ptr->mechanisms[10];
				eflags =  pt_ptr->mechanisms[11];
				eax =  pt_ptr->mechanisms[12];
				ecx =  pt_ptr->mechanisms[13];
				edx =  pt_ptr->mechanisms[14];
				ebx =  pt_ptr->mechanisms[15];
				esp =  pt_ptr->mechanisms[16];
				ebp =  pt_ptr->mechanisms[17];
				esi =  pt_ptr->mechanisms[18];
				edi =  pt_ptr->mechanisms[19];
				dprint("esp0: %08x ", pt_ptr->mechanisms[3]);
				dprint("ss0: %04x ", pt_ptr->mechanisms[4]);
				dprint("es: %04x ", pt_ptr->mechanisms[20]);
				dprint("cs: %04x ", pt_ptr->mechanisms[21]);
				dprint("ss: %04x ", pt_ptr->mechanisms[22]);
				dprint("ds: %04x\n", pt_ptr->mechanisms[23]);
				//dprint("pdbr: %08x ", pt_ptr->mechanisms[9]);
				dprint("eip: %08x ", pt_ptr->mechanisms[10]);
				dprint("eflags: %08x ", pt_ptr->mechanisms[11]);
				dprint("eax: %08x ", pt_ptr->mechanisms[12]);
				dprint("ecx: %08x ", pt_ptr->mechanisms[13]);
				dprint("edx: %08x ", pt_ptr->mechanisms[14]);
				dprint("ebx: %08x ", pt_ptr->mechanisms[15]);
				dprint("esp: %08x ", pt_ptr->mechanisms[16]);
				dprint("ebp: %08x ", pt_ptr->mechanisms[17]);
				dprint("esi: %08x ", pt_ptr->mechanisms[18]);
				dprint("edi: %08x ", pt_ptr->mechanisms[19]);
			}
			else
			{
				if (esp0 != pt_ptr->mechanisms[3]) dprint("esp0: %08x ", pt_ptr->mechanisms[3]);
				//dprint("pdbr: %08x ", pt_ptr->mechanisms[9]);
				if (eip != pt_ptr->mechanisms[10]) dprint("eip: %08x ", pt_ptr->mechanisms[10]);
				if (eflags != pt_ptr->mechanisms[11]) dprint("eflags: %08x ", pt_ptr->mechanisms[11]);
				if (eax != pt_ptr->mechanisms[12]) dprint("eax: %08x ", pt_ptr->mechanisms[12]);
				if (ecx != pt_ptr->mechanisms[13]) dprint("ecx: %08x ", pt_ptr->mechanisms[13]);
				if (edx != pt_ptr->mechanisms[14]) dprint("edx: %08x ", pt_ptr->mechanisms[14]);
				if (ebx != pt_ptr->mechanisms[15]) dprint("ebx: %08x ", pt_ptr->mechanisms[15]);
				if (esp != pt_ptr->mechanisms[16]) dprint("esp: %08x ", pt_ptr->mechanisms[16]);
				if (ebp != pt_ptr->mechanisms[17]) dprint("ebp: %08x ", pt_ptr->mechanisms[17]);
				if (esi != pt_ptr->mechanisms[18]) dprint("esi: %08x ", pt_ptr->mechanisms[18]);
				if (edi != pt_ptr->mechanisms[19]) dprint("edi: %08x ", pt_ptr->mechanisms[19]);
			}
			if (pt_ptr->mechanisms[20] != 0x10) dprint("es: %04x ", pt_ptr->mechanisms[20]);
			if (pt_ptr->mechanisms[21] != 0x8) dprint("cs: %04x ", pt_ptr->mechanisms[21]);
			if (pt_ptr->mechanisms[22] != 0x10) dprint("ss: %04x ", pt_ptr->mechanisms[22]);
			if (pt_ptr->mechanisms[23] != 0x10) dprint("ds: %04x ", pt_ptr->mechanisms[23]);
			if (pt_ptr->mechanisms[23] != 0x10) dprint("ss0: %04x ", pt_ptr->mechanisms[4]);
			dprint("\n");
		}
		pt_ptr++;
	}
	return 1;
}

int InterpretError(char *error_str)
{
	printf("Interpret reason & error codes.\n");
	char *pEnd;
	unsigned long reason = strtoul(error_str, NULL, 16);
	long sr = (0 - reason);
	printf("Converted string = 0x%x - ", reason);
		//printf("sr = 0x%x/%d (reason 0x%x/%d)...(str offset %d)\n", sr, sr, reason, reason, sr + ERROR_OFFSET);
		//printf("Offsets: RO 0x%x/%d, RL 0x%x/%d, EO 0x%x/%d, EL 0x%x/%d\n\n", REASON_OFFSET, REASON_OFFSET, REASON_LAST, REASON_LAST, ERROR_OFFSET, ERROR_OFFSET, ERROR_LAST,		ERROR_LAST);
	char sign = '+';
	char rstring[50];
	if (reason >= REASON_OFFSET && reason <= REASON_LAST)
	{
		printf("Valid reason code.");
		strcpy(rstring, reason_strings[reason - REASON_OFFSET]);
	}
	else if (reason <= ERROR_OFFSET && reason >= ERROR_LAST)
	{
		printf("Valid error code.");
		sign = '-';
		reason = 0 - reason;
		strcpy(rstring, error_strings[sr + ERROR_OFFSET]);
		//printf("rstring %s, error string %s\n", rstring, error_strings[8]);
	}
	else
	{
		sign = '-';
		printf("not a valid error or reason code.\n");
		printf("Reason codes are between 0x%x and 0x%x.\n", REASON_OFFSET, REASON_LAST);
		printf("Error codes are between 0x%x (-0x%x) and 0x%x (-0x%x) .\n", ERROR_OFFSET, ERROR_OFFSET*-1, ERROR_LAST, ERROR_LAST*-1);
		return 0;
	}
	//printf("sign/reason,rstring: %c0x%02x %s.\n", sign, reason, rstring);
	printf(" Result: %c0x%02x %s.", sign, reason, rstring);
	return 1;
}

/* END OF FILE */
