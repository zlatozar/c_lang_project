///////////////////////////////////////////////////////////////////////////////////////////////////
/* Create Image tool
	createimage.c

	createimage creates a kernel image that uses the file server. It appends the boot loader,
	kernel and user programs and files together into a single binary image for a virtual machine.

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

#include "../../kernel/mechanism/pc/boot/bootdata.h"
#include "../../kernel/mechanism/pc/kernel_ia32.h"
#include "../../kernel/policy/kernel.h"

#include "../colors.h"

#define FIRST_USER_BLOCK	5
#define MATCHING_BITS		4
#define TRANSPARENT_BITS	5
#define FILE_TABLE_SIZE		2
// file table size is 2 blocks.

#define FILE_PADDING		0x05
/* This is used to fill the file system between the end of an file's binary image and the next
 * block.*/

#define FILE_TABLE_PADDING	0x81

#define ERROR_OPENING_OUT_FILE		-4
#define ERROR_OPENING_USER_FILE		-5
#define ERROR_OPENING_BOOT_FILE		-6

void CreateBootTableEntry(char *filename, int priority_or_size);
void AddBootFile(char *filename);

void AddFileTableEntry(char *filename, int start, int size);
void AddUserFile(char *filename);

int  GetPosition(void);
int  SetPosition(int pos);

int  Read8(void);
int  Read32(void);
int  Write8(int c);
int  Write32(int c);

void PadFileTable(int no_infiles);
void PadUserFile(void);

void WriteBootParams(void);

void PrintLog(const char * format, ...);
void PrintHelp(int no_args);

boot_table_t	g_bootT[5]; //kernel, idle, coordinator, disk driver, file table.
int			g_bootT_entry = 0;

long		g_file_table_start;
long		g_file_table_next;

// Offsets in target image:
FILE	*g_file_in_ptr;		//temp pointer.
FILE	*g_file_out_ptr;	// for new file.
FILE	*g_file_log_ptr;	// log of output is created in log_name
char	*g_log_name = "createimage.log";

int		g_current_block;	// current block (ie next free block).
long	g_current_position;	// current position (in bytes) in g_file_out_ptr.
int		g_verbose	= 0;
char	g_signature	= '@';	// this is used at the end of the 'outfile'.

int main(int argc, char* argv[])
{
	// 0createimage 1[options] 2outfile 3bootsector 4kernel 5-n:user-infile-list
	// debug:
	//  gdb --args createimage.exe -v test.img boot.bin kernel.bin user1.bin user2.bin user3.bin user1.bin
	
	g_current_position	= 0;
	g_current_block		= 0;
	int num_users		= argc - 5;

	// check arguments
	if (argc < 5)
		PrintHelp(argc-1);
	
	printf(BLUE "Create kernel image %s using: %s, %s and %d users\n" NORMAL,argv[2],argv[3],argv[4],num_users);
	
	// Set to verbose, or generate log file. The log file pointer (g_file_log_ptr) defaults to standard out.
	g_file_log_ptr = stdout;
	
	if (strcmp(argv[1], "-v") == 0)
	{
		g_verbose = 1;
	}
	else if (!strcmp(argv[1], "-l")) // create log file
	{
		g_verbose		= 1;
		g_file_log_ptr	= fopen(g_log_name, "w+b");
		/* read and write binary mode (truncates file to zero length or creates new file)*/
	
		if (g_file_log_ptr == NULL)
		{
			printf("\topen %s failed\n", g_log_name);
			g_file_log_ptr = stdout;
		}
	}
	else if (strcmp(argv[1], "-h") == 0)
	{
		PrintHelp(argc - 1);
	}
	else
	{
		PrintHelp(argc - 1);
	}

	//open output file
	g_file_out_ptr = fopen(argv[2], "w+b");
	/* read and write binary mode (truncates file to zero length or creates new file)*/
	
	if (g_file_out_ptr == NULL)
	{
		printf("\topen %s failed\n", argv[2]);
		exit(ERROR_OPENING_OUT_FILE);
	}

	AddBootFile(argv[3]); // add bootsector.

	/* The following entries are written from the current position, which is incremented each time
	 * according to the input file size. The boot table and boot parameters should fit inside one
	 * block (1KB), we need to increment the current position/block values.
	 * NB: This does not affect where these boot files are placed in the output file, that is
	 * determined by g_file_out_ptr.*/
	g_current_block++;
	
	CreateBootTableEntry(argv[4],		NO_PRIORITY);		// kernel.bin
	CreateBootTableEntry(argv[5],		NO_PRIORITY);		// idle.bin
	CreateBootTableEntry(argv[6],		FAST_PRIORITY);		// server.bin
	CreateBootTableEntry(argv[7],		NORMAL_PRIORITY);	// disk.bin
	CreateBootTableEntry("file table",	FILE_TABLE_SIZE);	// file table, 2 blocks in size.
	WriteBootParams();
	
	/* Now that the boot table entries are written for the boot users, write their file data to the
	 * output image.*/
	for (int i = 4; i < 8; i++)
		AddBootFile(argv[i]);
	
	/* Add the file table. This includes the boot users - the first entry in the file table is for
	 * Idle. We really just want to reserve space for the file table here. We also need a way of
	 * keeping track of which entry we're up to.*/
	g_file_table_start = GetPosition();
	
	SetPosition(g_file_table_start + FILE_TABLE_SIZE * BLOCK_SIZE);
	
	// Add the boot files to the file table.
	g_file_table_next = g_file_table_start;
	
	PrintLog("Add the boot files to the file table, except for the kernel.\n");
	
	for (int i = 5; i < 8; i++)
		AddFileTableEntry(argv[i], g_bootT[i - 4].block, g_bootT[i - 4].size);

	// Add the remaining users. This adds the user file data, and creates the file table entry.
	PrintLog("Add the remaining user files to the output image and the file table.\n");
	
	for (int i = 8; i < argc; i++)
	{
		AddUserFile(argv[i]);
	}
	
	g_current_block += FILE_TABLE_SIZE;
	PadFileTable(num_users - 3);
	/* Fill empty slots, that is pad table by number of slots minus number of slots used.*/

	// put signature at end of outfile.
	for (int i = 0; i < 32; i++)
		fputc(g_signature, g_file_out_ptr);
	
	fclose(g_file_out_ptr);
	PrintLog("createimage success!\n\n");
	
	if (g_file_log_ptr != stdout)
		fclose(g_file_log_ptr);
	
	return 0;
}

void AddFileTableEntry(char *filename, int start, int size)
{
	int save_position = GetPosition();
	
	SetPosition(g_file_table_next);
	
	/* Copy the filename into a buffer 24 char long. If the filename includes path information,
	 * then dump it and just copy the filename.*/
	int i = 0, j = 0;
	char filename_in_table[24];
	
	while (j < 24 && filename[i] != '\0')
	{
		char ch = filename[i];
		
		if (ch == '/')
		{
			j = 0;
			i++;
		}
		else
		{
			filename_in_table[j] = ch;
			i++;
			j++;
		}
	}
	
	filename_in_table[j] = '\0';
	i = 0;
	
	while (i < 24 && filename_in_table[i] != '\0')
	{
		fputc(filename_in_table[i], g_file_out_ptr);
		i++;
	}
	while (i < 24)
	{
		fputc('\0', g_file_out_ptr);
		i++;
	}
	
	Write32(start);
	Write32(size);
	
	PrintLog(BLUE "Add file: %s | table entry location 0x%x | block start 0x%x | size in blocks 0x%x.\n" NORMAL,
			filename_in_table, g_file_table_next, start, size);

	// After writing the entry, save the position for when we write the next entry.
	g_file_table_next = GetPosition();
	
	SetPosition(save_position);

	return;
}

/* Create a boot table entry with the #filename and its #priority.
 * If the entry is the pointer to the system file table, then the #filename should be "file table"
 * and the #priority_or_size parameter specifies the size of the file table.*/
void CreateBootTableEntry(char *filename, int priority_or_size)
{
	int size = priority_or_size;
	
	if (strcmp(filename, "file table")==0)
	{
		Write32(size);
		Write32(g_current_block);
		Write32(BOOT_TABLE_OBJECT_STATUS_0); // priority
		Write32(BOOT_TABLE_OBJECT_STATUS_1); // first_instruction
	}
	else
	{
		g_file_in_ptr	= fopen(filename, "rb");
		fseek(g_file_in_ptr, 0, SEEK_END);
		
		long pos		= ftell(g_file_in_ptr);
		size			= (pos/1024)+1;
		
		if (pos % 1024 == 0)
			size--;
		
		Write32(size);
		Write32(g_current_block);
		Write32(priority_or_size);				// priority
		Write32(BOOT_TABLE_FIRST_INSTRUCTION);	// first_instruction
		g_bootT[g_bootT_entry].block	= g_current_block;
		g_bootT[g_bootT_entry].size		= size;
	}
	
	PrintLog(GREEN "Add boot entry: %s | block start 0x%x (0x%x) | end 0x%x (0x%x) | size in blocks 0x%x | first instruction 0x%x.\n" NORMAL,
			filename, g_current_block, g_current_block * BLOCK_SIZE, g_current_block + size,
			(g_current_block + size) * BLOCK_SIZE, size, BOOT_TABLE_FIRST_INSTRUCTION);

	g_current_block += size;
	g_bootT_entry++;
	
	return;
}

void WriteBootParams(void)
{
	int i;

	/* Write some boot parameters to the boot table. The server/coordinator can use this to jump to
	 * the user parameters.*/
	Write32(NUM_BOOT_PARAMS * 4);

	Write32(MAX_PROCESSES);
	Write32(VERSION_SHIFT);
	Write32(MATCHING_BITS);
	Write32(TRANSPARENT_BITS);
	Write32(BLOCK_SIZE);
	Write32(NUM_IOPORTS);
	Write32(NUM_MAPS);
	
	// fill the rest of the boot table:
	int padding_dwords = (BLOCK_SIZE - 4 * 4 * 5) / 4;
	/* 4*4*5 is 4 dword length entries * 5 boot table entries, but Write32 uses dwords (4 bytes) so
	 * divide the whole thing by 4.*/
	
	padding_dwords -= NUM_BOOT_PARAMS; // subtract the number of parameters written from just above.
	
	for (i = 0; i < padding_dwords; i++)
		Write32(BOOT_TABLE_PADDING);
	
	return;
}

void AddUserFile(char *filename)
{
	int verbose_save	= g_verbose;
	g_verbose			= 1;
	g_file_in_ptr		= fopen(filename, "rb"); /*read binary mode*/
	
	if (g_file_in_ptr == NULL)
	{
		printf("\tfile %s open failed\n", filename);
		fclose(g_file_in_ptr);
		exit(ERROR_OPENING_USER_FILE);
	}
	
	// Get the current position, where the file will be written.
	int start_block = GetPosition() / BLOCK_SIZE;
	
	// Write the user file to the output image.
	int c = fgetc(g_file_in_ptr);

	while (c != EOF)
	{
		fputc(c, g_file_out_ptr);
		c = fgetc(g_file_in_ptr);
		// the order of these two might need to be swapped - this order writes the EOF. Maybe that's good?!
	}

	PadUserFile();
	
	// determine size of file.
	int pos				= ftell(g_file_in_ptr);
	int size_in_blocks	= (pos / BLOCK_SIZE) + 1;
	
	if (pos % BLOCK_SIZE == 0)
		size_in_blocks--;

	g_current_block		+= size_in_blocks;
	g_current_position	 = GetPosition();
	
	AddFileTableEntry(filename, start_block, size_in_blocks);
	
	fclose(g_file_in_ptr);
	g_verbose = verbose_save;
	
	return;
}

void AddBootFile(char *filename)
{
	int i = 0;
	int c;
	
	g_file_in_ptr = fopen(filename, "rb"); /*read binary mode*/
	
	if (g_file_in_ptr == NULL)
	{
		printf("\tfile %s open failed\n", filename);
		fclose(g_file_in_ptr);
		exit(ERROR_OPENING_BOOT_FILE);
	}
	
	// Save the starting position for the boot file simply for our log message.
	int startpos	= GetPosition();
	c				= fgetc(g_file_in_ptr);
	
	while (c != EOF)
	{
		fputc(c, g_file_out_ptr);
		c = fgetc(g_file_in_ptr);
	}
	
	// Need to write enough characters to align the added file to the next 1KB block. 0x05 is
	// written, but it could be anything, eg 0x00 or 0x90 (NOP instruction).
	
	PadUserFile();
	
	g_current_position	= GetPosition();
	g_current_block		= g_current_position / BLOCK_SIZE;
	
	PrintLog("\tAddBootFile %s from 0x%X to 0x%X\n", filename, startpos, GetPosition());
	fclose(g_file_in_ptr);
	
	return;
}

/* Pad the output for the currently written user file until the end of the current block.*/
void PadUserFile(void)
{
	long pos	= ftell(g_file_in_ptr);
	
	long append	= 1024 - pos % 1024;
	
	if (pos % 1024 == 0)
		append -= 1024;
	
	fseek(g_file_out_ptr, 0, SEEK_END);
	
	for (int i = 0; i < append; i++)
		fputc(FILE_PADDING, g_file_out_ptr);
}

/*
 * Get the current position in the output file.
 */
int GetPosition(void)
{
	return ftell(g_file_out_ptr);
}

/*
 * Set the current position in the output file to 'pos'.
 * Updates g_current_position to 'pos'.
 */
int SetPosition(int pos)
{
	g_current_position = pos;
	fseek(g_file_out_ptr, pos, SEEK_SET);
	
	return pos;
}

int Read8(void)
{
	int value;
	fread(&value, 1, 1, g_file_out_ptr);
	
	return value;
}

int Read32(void)
{
	int value;
	fread(&value, 4, 1, g_file_out_ptr);
	
	return value;
}

/*
 * Write 32-bit integer to the file output stream.
 * This increments the g_current_position by 4.
 */
int Write32(int c)
{
	int number_ascii[4], i;
	
	number_ascii[0] = c & 0xFF;
	number_ascii[1] = (c>>8) & 0x00FF;
	number_ascii[2] = (c>>16) & 0x0000FF;
	number_ascii[3] = (c>>24) & 0x000000FF;
	
	for (i = 0; i < 4; i++)
		fputc(number_ascii[i], g_file_out_ptr);
	
	g_current_position += 4;
	
	return 1;
}

/* Write 8-bit integer to the file output stream.
 * This increments the g_current_position by 1.*/
int Write8(int c)
{
	int number_ascii = c & 0xFF;

	fputc(number_ascii, g_file_out_ptr);

	g_current_position++;

	return 1;
}

void PadFileTable(int no_infiles)
{
	int c, i;
	
	c = FILE_TABLE_SIZE - ((no_infiles) * 32 * 4);
	
	for(i = 0; i < c; i++)
		fputc(FILE_TABLE_PADDING, g_file_out_ptr);
	
	g_current_position += c;
	
	return;
}

void PrintLog(const char * format, ...)
{
  va_list args;
  
  if (g_verbose)
  {
	  va_start (args, format);
	  vfprintf (g_file_log_ptr, format, args);
	  va_end (args);
  }
}

void PrintHelp(int no_args)
{
		printf("createimage [options] outfile bootsector kernel user-infile-list...\n");
		printf("\tyou only provided %d args\n",no_args);
		printf("\toptions: -s silent, -v verbose (prints messages on stdout, -l outputs messages log file 'createimage.log', -h this help\n");
		printf("\t(You must provide an option here, so if you don't really want any use -s)\n");
		printf("\toptions: -Hx specifies number (x) of handlers in user list. Follow this option with the users.\n");
		printf("\toptions: -Lx specifies number (x) of libraries in user list. Follow this option with the users.\n");
		printf("\toptions: -Px specifies number (x) of ordinary programs in user list. Follow this option with the users.\n");
		exit(-1);
}
