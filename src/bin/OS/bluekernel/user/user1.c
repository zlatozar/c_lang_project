///////////////////////////////////////////////////////////////////////////////////////////////////
/* User test program #1
	user1.c

	This program sends the alphabet to process ID 0x7001 and is expected to be at 0x6001.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "lib/syscall.h"
#include "lib/syscallext.h"
#include "lib/stdlib.h"

pID_t	this_pID;
pID_t	server_pID;
pID_t	video_pID;

int kmain(void);
int	Open(void);

int kmain(void)
{
	Open();
	Timer(0x1000);
	
	char c = 'a';
	printc(c);
	
	while (1)
	{
		Send(0x7001, (uint32_t*)&c, 1, 0);
		
		printc(c);
		
		if (c == 'z')
			c = 'a';
		else
			c++;
	}
	return 0;
}

int Open(void)
{
	this_pID	= GetCurrentPid();

	server_pID	= GetCoordinator();

	GetSystemInfo();
	
	kprintf("user1:");
	
	return 1;
}

/* END OF FILE */
