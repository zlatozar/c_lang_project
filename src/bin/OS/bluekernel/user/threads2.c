///////////////////////////////////////////////////////////////////////////////////////////////////
/* Threads test program #2
  threads2.c

  This program receives characters from process ID 0x6001 and its threads. It should run at
  0x7001 to work with threads1.c

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
pID_t user1_pID;
pID_t video_pID;
pID_t server_pID;

void  TestThreads(void);
int   Open(void);

int
kmain(void)
{
  Open();
  TestThreads();

  return 0;
}

void
TestThreads(void)
{
  char  c   = 'A';
  pID_t sender  = 0;

  printc(c);

  while (1) {
    sender = Receive(user1_pID, (uint32_t*)&c, sizeof(char) + TTHREAD + RTHREAD, 0);

    if (sender > 0)
    { printx(sender >> 12, 1); }
    else
    { printx(sender, 4); }

    printc(c);

    Timer(0x800);
  }
  return;
}

int
Open(void)
{
  this_pID  = GetCurrentPid();
  user1_pID = 0x6001;
  server_pID  = GetCoordinator();

  GetSystemInfo();

  kprintf("threads2.");

  return 1;
}

/* END OF FILE */
