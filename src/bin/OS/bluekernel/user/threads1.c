///////////////////////////////////////////////////////////////////////////////////////////////////
/* Threads test program #1
  threads1.c

  This program sends a character of the alphabet, one at a time, to process ID 0x7001 and any of
  its threads.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_development_testing Example User Code
// @A+

#include "lib/syscall.h"
#include "lib/syscallext.h"
#include "lib/stdlib.h"

pID_t this_pID;
pID_t threads2_pID;
pID_t request_pID;
pID_t server_pID;
pID_t video_pID;

int   kmain(void);
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
  char c = 'a';

  while (1) {
    Send(threads2_pID, (uint32_t*)&c, sizeof(char) + TTHREAD + RTHREAD, 0);

    if (c == 'z')
    { c = 'a'; }
    else
    { c++; }

    Timer(400);
  }
  return;
}

int
Open(void)
{
  this_pID    = GetCurrentPid();
  server_pID    = GetCoordinator();
  threads2_pID  = 0x7001;

  GetSystemInfo();

  kprintf("thread 1.");

  return 1;
}
// @A-

/* END OF FILE */
