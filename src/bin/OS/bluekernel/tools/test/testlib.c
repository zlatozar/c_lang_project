/*
 * This file is used for quick prototyping of code for the kernel & server.
 * 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>

char *kstrcpy (char *destination, const char *source);
int atoi (const char * str);

int main(int argc, char **argv)
{
	printf("this many args %d\n", argc);
	
	printf("str is %s = %d\n", argv[2], atoi(argv[2]));
	
	char str1[] = "hello there, yo!\n";
	char *str2;
	
	kstrcpy(str2, str1);
	
	printf("str1 is %s, str2 is %s\n", str1, str2);
	
	return 0;
}

char *kstrcpy (char *destination, const char *source)
{
    int i = 0;
    
	while (source[i] != 0)
    {
      destination[i] = source[i];
      i++;
    }
	
	return destination;
}

int atoi (const char * str)
{
	int i = 0;
	int sum = 0;
	int base = 10;
	
	if (str[0] == '0' && str[1] == 'x')
	{
		base = 16;
		i = 2;
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
		sum += (str[i] - offset);
		
		i++;
	}
	return sum;
}