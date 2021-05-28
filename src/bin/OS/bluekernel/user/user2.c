///////////////////////////////////////////////////////////////////////////////////////////////////
/* User test program #2
  user2.c

  This program receives characters from process 0x6001, and is expected to be at 0x7001.

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

pID_t this_pID;
pID_t server_pID;
pID_t video_pID;

int kmain(void);
int Open(void);

int
kmain(void)
{
  Open();

  while (1) {
    char c        = '0';

    pID_t request_pID = Receive(0x6001, (uint32_t*)&c, 1, 0);

    printx(request_pID >> VERSION_SHIFT, 1);
    printc(c);
    Timer(0x500);
  }
  return 0;
}

int
Open(void)
{
  video_pID = 0;
  this_pID  = GetCurrentPid();

  server_pID  = GetCoordinator();

  GetSystemInfo();

  kprintf("user2:");

  return 1;
}

/* END OF FILE */
