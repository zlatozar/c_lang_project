///////////////////////////////////////////////////////////////////////////////////////////////////
/* User test program #3
  user3.c

  A basic test program to trigger exceptions.

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

pID_t user3_pID = 0x1243;
char* str;
char* str2;
char* removeself;
uint32_t request[SERVER_MSG_SIZE];
float x, y;

pID_t server_pID;
pID_t video_pID;

int DoCodeDataFault(void);
int DoStackFault(void);
int DoException13(void);
int DoPageFault(void);
int DoProtectionFault(void);
int DoDivideByZeroError(void);
int FakeExceptionMsg(void);

int
kmain(void)
{
  int run = 0;

  user3_pID = GetCurrentPid();

  server_pID = GetCoordinator();

  GetSystemInfo();

  kprintf(str);

  while (1) {
    DoDivideByZeroError();

    run++;

    if (run == 4) {
      RemoveProcess(USE_CURRENT_PROCESS); //remove self.
    }
  }

  return 0;
}

char* str2 = ". .";
char* str = "user3 will cause an exception.\n";
char* removeself = "Remove self.";

int
DoDivideByZeroError(void)
{
  //simulate div0 exception:
  int q = 1, p = 0;
  q = q / p;

  return 1;
}

int
DoProtectionFault(void)
{
  //simulate protection fault:
  asm(
    "mov %ax, %ds\n\t"
    "mov %ax, (%ebx)\n\t"
  );
  return 1;
}

int
DoPageFault(void)
{
  //simulate page fault:
  char* str;
  str = (char*)0x50000000;
  *str = (char)0x35;
  return 1;
}

int
DoException13(void)
{
  //directly generate exception, although error code
  // is not pushed properly doing it this way:
  asm("int $13");

  return 1;
}

int
DoStackFault(void)
{
  //simulate stack fault: //1013 causes fault, 1012 doesn't.
  int j = 0;
  while (j < (1013 + 1024)) {
    asm(
      "mov $55, %eax\n\t"
      "push %eax\n\t"
    );
    j++;
  }

  return 1;
}

int
DoCodeDataFault(void)
{
  //simulate code/data fault:
  asm("movl $0x1fa02003, %eax"); //also test outside current page tables, eg 0x7ff02003
  asm("movl $0x55, (%eax)");

  return 1;
}

int
FakeExceptionMsg(void)
{
  uint32_t buffer[EXCEPTION_STACK_SIZE];
  buffer[EXC_NO] = 0x12;
  buffer[CR2] = 0x023;
  buffer[ERROR_CODE] = 0x1234;
  buffer[EIP] = 0x23;
  buffer[CS] = 0x1b;
  buffer[EFLAGS] = 0x10202006;
  buffer[ESP] = 0x7fffdfe0;
  buffer[SS] = 0x23;
  Send(0x85, buffer, EXCEPTION_STACK_SIZE, 0);
  return 1;
}

/* END OF FILE */
