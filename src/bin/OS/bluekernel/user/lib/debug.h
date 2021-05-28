///////////////////////////////////////////////////////////////////////////////////////////////////
/* Debugging macros
  debug.h

  Use these simple definitions to aid debugging in the Bochs emulator.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG asm("nop")
#define BREAK asm("xchg %bx, %bx")
#define DELAY for(; i < 2; i++)

/* the BREAK instruction is used by Bochs emulator debugger to break
 * execution and return to debugger command line. The following line in
 * Bochs configuration file has to be enabled:
 * magic_break: enabled=1 */

