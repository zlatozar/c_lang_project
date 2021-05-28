///////////////////////////////////////////////////////////////////////////////////////////////////
/* Basic Standard Library
	stdlib.c

	This file contains very crude versions of stdlib functions.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdlib.h"

extern pID_t video_pID;

/* This function takes a variable length character string and sends it to the video in fixed-sized
 * messages. If the string is longer than the fixed message size then multiple messages are sent
 * until the end of the string is reached.*/
void kprintf(const char *str)
{
	uint32_t msg[SERVER_MSG_SIZE];
	
	msg[MSG_TYPE]		= MSG_VIDEO_MSG;

	char *string;
	int  str_size		= 0;
	const char *str_ptr	= str;
	
	while (*str_ptr != 0 && *str_ptr != 10)
	{
		str_size++;
		str_ptr++;
	}
	
	int i = 0, j;
	
	while (str_size > 0)
	{
		string	= (char*)&msg[1];
		j		= 0;
		
		while (j < STRING_SIZE && str[i] != 0 && str[i] != 10)
		{
			string[j] = str[i];
			i++;
			j++;
		}
		str_size -= j;
		string[j] = 0;
		Send(video_pID, msg, SERVER_MSG_SIZE, 0);
	}
	
	return;
}

/* Print a single character to the video.*/
int printc(char c)
{
	uint32_t msg[SERVER_MSG_SIZE];
	
	msg[MSG_TYPE]	= MSG_VIDEO_MSG;
	msg[1]			= (uint32_t)(c & 0xFF);
	
	Send(video_pID, msg, SERVER_MSG_SIZE, 0);
	
	return 1;
}

/* Print a hexadecimal number with 'width' number of digits. This will truncate numbers
 * accordingly. eg 0x1234 with width 3 will print as 234.*/
int printx(uint32_t num, int width)
{
	uint32_t msg[SERVER_MSG_SIZE];
	msg[MSG_TYPE]	= MSG_VIDEO_MSG;
	char *string	= (char*)&msg[1];
	
	if (width > 8)
		width = 8;
	
	for(int i = width; i > 0; i--)
	{
		int temp = (num % 16);
		
		if (temp > 9)
			string[i - 1] = temp - 10 + 'A';
		else
			string[i - 1] = temp + '0';
		
		num = num / 16;
	}
	
	// Append a null terminator to the string in the message.
	string[width] = 0;
	
	Send(video_pID, msg, SERVER_MSG_SIZE, 0);
	
	return OK;
}

int atoi(const char *str)
{
	int i		= 0;
	int sum		= 0;
	int base	= 10;
	
	if (str[0] == '0' && str[1] == 'x')
	{
		base	= 16;
		i		= 2;
	}
	
	while (str[i] != 0)
	{
		char offset = '0';
		
		if (str[i] >= '0' && str[i] <= '9')
			offset = '0';
		else if (base == 16 && str[i] >= 'a' && str[i] <= 'f')
			offset = 'a' - 10;
		else if	(base == 16 && str[i] >= 'A' && str[i] <= 'F')
			offset = 'A' - 10;
		else
			return 0;

		sum = sum * base;
		sum += str[i] - offset;
		
		i++;
	}
	return sum;
}

/* Basically the standard strcmp function - returns 0 if successful, 1 if the strings don't match.*/
int kstrcmp (const char *str1, const char *str2)
{
    int i = 0;
	
    while (str1[i] != 0 || str2[i] != 0)
    {
      if (str1[i] != str2[i])
		  return 1;
	  
      i++;
    }
    return 0;
}

/* Copies 'source' string to 'destination' location, and returns the destination location.*/
char* kstrcpy (char *destination, const char *source)
{
    int i = 0;
    
	while (source[i] != 0)
    {
      destination[i] = source[i];
      i++;
    }
	
	return destination;
}

/* END OF FILE */
