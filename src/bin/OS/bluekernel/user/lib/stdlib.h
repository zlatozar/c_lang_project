///////////////////////////////////////////////////////////////////////////////////////////////////
/* Header for standard library
	stdlib.h

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __STDLIB_H
#define __STDLIB_H

#include "syscall.h"
#include "../../kernel/mechanism/pc/resources_pc.h"
#include "../../server/server.h"

// string size is limited by msg size -1 first word is msg type, and *4 because 1 word = 4 chars.
#define STRING_SIZE				((SERVER_MSG_SIZE - 1) * 4)

#define READ_IRR_PORT_RES		0x10
#define PIC1_COMMAND_PORT_RES	0x11
#define PIC2_COMMAND_PORT_RES	0x12
#define READ_ISR_PORT_RES		0x13
#define PIC1_DATA_PORT_RES		0x14

void kprintf(const char *str);
int	 printc(char c);
int  printx(uint32_t num, int width);
int  atoi(const char *str);
int  kstrcmp(const char *str1, const char *str2);
char *kstrcpy(char *destination, const char *source);

#endif

/* END OF FILE */
