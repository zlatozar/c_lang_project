///////////////////////////////////////////////////////////////////////////////////////////////////
/* Video Driver
	video.c

	This is a basic video driver for CGA mode with 80x25 characters.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../server/server.h"
#include "../lib/syscall.h"
#include "../lib/syscallext.h"
#include "../lib/stdlib.h"
#include "../lib/debug.h"
#include "video/vga.h" // Bochs vga.h definitions.

#define VIDEO_BASE				0xF000
#define VIDEO_PHYS				0xB8000 // 0xA0000  
#define VIDEO_PHYS_END			0x000BFFFF
#define NUM_VIDEO_PAGES			((VIDEO_PHYS_END - VIDEO_PHYS + 1)/PAGE_SIZE)
// video memory may extend to 0x000BFFFF, which gives 0x20000, or 128KB, or 32 pages.

#define PROCESS_ROWS			13
// Change this value to move individual process rows up or down.

#define ROW_MAX					25
#define COL_MAX					80
#define VIDEO_MIN				VIDEO_BASE
#define VIDEO_MAX				(VIDEO_MIN + ROW_MAX * COL_MAX * 2)
#define PROCESS_ID_OFFSET		0x80
#define KERNEL_ZERO_OFFSET		0x80100000u
#define VIDEO_KERNEL			(VIDEO_PHYS + KERNEL_ZERO_OFFSET)
// src virtual address for video to map to video device.

#define VIDEO_MAPPING			VIDEO_BASE
//dst virtual address of video device.

#define WHITE					7
#define BLUE					3

char *video;
char *video_base				= (char*)(VIDEO_BASE);
int  video_limit				= 80 * 25 * 2;
int  x;
char colour;

uint32_t	num_procs;
uint32_t	user_buffers[20];
uint32_t	request[SERVER_MSG_SIZE];
uint32_t	reply[SERVER_MSG_SIZE];
int32_t		request_pID;
uint32_t	d[MSG_MAX_LENGTH];
uint32_t	src, dest;
pID_t		pager_pID;
pID_t		this_pID;
pID_t		server_pID;
pID_t		video_pID;

int DoRequest(int32_t request_pID);
int Open(void);
int InitialiseVideoMemory();
int InitVideoDisplay(void);

int VideoCharOut(uint32_t pID, char ch);
int vprintf(int32_t src, char *str);
int vprintx(uint32_t num, int width);
int TellCoordVideoReady(void);

int kmain(void)
{
	Open();
	
	while (1)
	{
		request_pID = Receive(ANY, request, SERVER_MSG_SIZE, 0);
		DoRequest(request_pID);
	}
}

int DoRequest(int32_t request_pID)
{
	switch (request[MSG_TYPE])
	{
		case MSG_VIDEO_MSG:
		{
			char *string = (char*)&request[1];
			
			while (*string != 0 && string < (char*)&request[SERVER_MSG_SIZE])
			{
				VideoCharOut(request_pID, *string);
				string++;
			}
			
			break;
		}
		case MSG_SET :
		{
			colour = request[1];
			break;
		}
		default :
		{
			break;
		}
	}
	
	return 1;
}

int Open(void)
{
	server_pID	= GetCoordinator();

	colour		= WHITE;
	this_pID	= GetCurrentPid();
	
	if (!InitialiseVideoMemory())
		while (1);
		
	InitVideoDisplay();
	
	for(int i = 0; i < MSG_MAX_LENGTH; i++)
		d[i] = i;
	
	vprintf(this_pID, "video driver");
	
	TellCoordVideoReady();
	
	return OK;
}

int InitialiseVideoMemory()
{
	num_procs	= GetNumProcesses();
	int result	= MapPageRequest(KERNEL_PID, VIDEO_KERNEL, USE_CURRENT_PROCESS, VIDEO_MAPPING,
								 USER_RWX);
	
	return result;
}

// Initialise a basic video display.
int InitVideoDisplay(void)
{
	int i;
	
	for (i = 0; i < 20; i++)
		user_buffers[i] = 6;
	
	video = video_base;
	
	/* print process slot on edge */
	for (i = PROCESS_ROWS; i < ROW_MAX; i++)
	{
		video = (char*)(VIDEO_BASE + i * COL_MAX * 2);
		
		if ((i - PROCESS_ROWS) < 10)
			*video = '0' + i - PROCESS_ROWS;
		else
			*video = 'A' + i - PROCESS_ROWS - 10;
	}
	video = (char*)(VIDEO_BASE + COL_MAX * 2);
	
	for(i = 0; i < 38; i++)
	{
		*video	= '-';
		video	+= 2;
	}
	
	video = (char*)VIDEO_BASE;
	
	return OK;
}

// Output a single character at the current cursor.
int VideoCharOut(uint32_t pID, char ch)
{
	pID = pID >> VERSION_SHIFT;
	
 	if (video >= (char*)(VIDEO_BASE + VIDEO_MAX))
 	{
 		video = (char*)VIDEO_BASE;

 		// *Note 1: see code at end
	 	x++;
		
	 	if (x == 26)
			x = 0;
 	}
	
	if (ch == 10 || ch == 13) //line feed/eol/carriage return.
	{
		video = (char*)(((video - (char*)VIDEO_BASE) / 160 + 1) * 160 + (char*)VIDEO_BASE);
		
		if (video >= (char*)(VIDEO_BASE + VIDEO_MAX) || video < (char*)VIDEO_BASE)
			video = (char*)VIDEO_BASE;
	}
	else
	{
		if (pID != 0)
		{
			video = (char*)(VIDEO_BASE + (pID + PROCESS_ROWS) * 160 + user_buffers[pID]);
			
			user_buffers[pID] += 2;
			
			if (user_buffers[pID] == 160)
				user_buffers[pID] = 6;
		}
		if (video < (char*)VIDEO_MIN || video >= (char*)VIDEO_MAX)
		{
			DEBUG;
		}
		else
		{
			*video = ch;
			video++;
			*video = colour;
			video++;
		}
	}
	
	return OK;
}

int vprintf(int32_t src, char *str)
{
	while (*str != 0)
		VideoCharOut(src, *str++);
	
	return OK;
}

int vprintx(uint32_t num, int width)
{
	char c[8];
	int  temp;
	int i;
	
	for(i = 0; i < 8; i++)
	{
		temp = (num % 16);
		
		if (temp > 9)
			c[i] = temp - 10 + 'A';
		else
			c[i] = temp + '0';
		
		num = num / 16;
	}
	
	for(i = width - 1; i >= 0; i--)
		VideoCharOut(this_pID, c[i]);
	
	return OK;
}

int TellCoordVideoReady(void)
{
	request[MSG_TYPE]		= MSG_SET;
	request[MSG_USER_TYPE]	= MSG_FILE_STATUS;
	request[2]				= USER_STATUS_INIT;
	Send(server_pID, request, SERVER_MSG_SIZE, 100);
	return OK;
}

/* END OF FILE */
