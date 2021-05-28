///////////////////////////////////////////////////////////////////////////////////////////////////
/* Generic Exception Handler
  excepter.c

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "server.h"
#include "../user/lib/syscall.h"
#include "../user/lib/syscallext.h"
#include "../user/lib/stdlib.h"

int   kmain(void);
int   DoRequest(pID_t request_pID);
int   Open(void);
void  PrintName(void);
int   FixException(pID_t request_pID);
int   PrintException(pID_t request_pID);

uint32_t  request[SERVER_MSG_SIZE];
uint32_t  buffer[EXCEPTION_STACK_SIZE];
pID_t   server_pID, video_pID;

int
kmain(void)
{
  Open();

  while (1) {
    pID_t request_pID = Receive(ANY, request, SERVER_MSG_SIZE, 0);
    DoRequest(request_pID);
  }
}

int
DoRequest(pID_t request_pID)
{
  switch (request[MSG_TYPE]) {
    case EXCEPTION_SYSCALL: {
      PrintException(request_pID);
      FixException(request_pID);
      break;
    }
    case MSG_PRINT_NAME: {
      PrintName();
      break;
    }
  }
  return 1;
}

int
Open(void)
{
  server_pID = GetCoordinator();

  GetSystemInfo();

  PrintName();

  return 1;
}

void
PrintName(void)
{
  kprintf("exception handler");
}

int
FixException(pID_t request_pID)
{
  kprintf("remove excepting process ");
  printx(request_pID >> 12, 1);

  request[MSG_TYPE] = MSG_CLOSE_PID;
  request[1]      = request_pID;

  Send(server_pID, request, SERVER_MSG_SIZE, 0);

  return 1;
}

int
PrintException(pID_t request_pID)
{
  // Buffer is: EXC_NO|CR2|error code|EIP|CS|EFLAGS|ESP|SS
  kprintf("pID ");
  printx(request_pID >> 12, 2);

  kprintf(" exc no.: ");
  printx(request[EXC_NO], 2);

  kprintf(" eip: ");
  printx(request[EIP], 8);

  kprintf(" eflags ");
  printx(request[EFLAGS], 8);
  kprintf(". ");

  return 1;
}

/* END OF FILE */
