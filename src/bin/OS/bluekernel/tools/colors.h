///////////////////////////////////////////////////////////////////////////////////////////////////
/* Color definitions for using with printf(), etc.
	colors.h

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef COLORS_H
#define COLORS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* \x1B is hex for ESC. Format for color string is:
 * <ESC>[{attr};{fg};{bg}m
 * attributes:
 * RESET  0, BRIGHT 1, DIM  2, UNDERLINE  4, BLINK  ?, REVERSE  7, HIDDEN  8 (3 is ?,
 * 5 puts weird background on (maybe blink rate is too quick), 6 is ?)
 * 
 * colors:
 * BLACK  0, RED 1, GREEN 2, YELLOW 3, BLUE 4, MAGENTA 5, CYAN 6, WHITE 7
 * for fg (foreground) + 30, for bg (background) + 40.
 * The following function could be used:
 * void textcolor(int attr, int fg, int bg)
{	char command[13];

	//Command is the control command to the terminal
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}
*/

#define NORMAL				"\x1B[0m"
#define RED					"\x1B[0;31m"
#define GREEN				"\x1B[32m"
#define YELLOW				"\x1B[33m"
#define BLUE				"\x1B[1;34m"
#define MAGENTA				"\x1B[35m"
#define CYAN				"\x1B[36m"
#define WHITE				"\x1B[37m"
#define RED_UNDERLINE		"\x1B[4;31m"
#define GREEN_UNDERLINE		"\x1B[4;32m"
#define YELLOW_UNDERLINE	"\x1B[4;33m"
#define BLUE_UNDERLINE		"\x1B[4;34m"
#define MAGENTA_UNDERLINE	"\x1B[4;35m"
#define CYAN_UNDERLINE		"\x1B[4;36m"
#define WHITE_UNDERLINE		"\x1B[4;37;40m"

#ifdef __cplusplus
}
#endif

#endif /* COLORS_H */

