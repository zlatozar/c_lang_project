///////////////////////////////////////////////////////////////////////////////////////////////////
/* System Call Extensions
  syscallext.c

  This file contains functions that package system calls into more convenient function names.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "syscallext.h"

extern pID_t server_pID;
extern pID_t video_pID;

// The following functions map onto system calls, but send the requests via the coordinator.

pID_t
GetCoordinator(void)
{
  uint32_t properties[2];

  properties[0] = K_COORD;
  int32_t result  = GetProcess(0, properties);

  if (result == OK)
  { return properties[0]; }
  else
  { return result; }
}

int
SetAsCoordinator(void)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[MSG_TYPE]   = MSG_SET;
  msg[MSG_USER_TYPE]  = MSG_SET_COORDINATOR;
  msg[5]        = USE_CURRENT_PROCESS;

  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);

  return msg[0];
}

void
GetSystemInfo(void)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[MSG_TYPE] = MSG_INFO;

  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);

  // the video_pID is a global variable.
  video_pID = msg[0];
}

int
SetProcessRequest(pID_t pID, int property, uint32_t value)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[MSG_TYPE]   = MSG_SET;
  msg[MSG_USER_TYPE]  = SET_PROCESS_SYSCALL;
  msg[2]        = pID;
  msg[3]        = property;
  msg[4]        = value;
  msg[5]        = 0;
  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);
}

int32_t
AddPageRequest(pID_t pID, uint32_t virtual_address, uint32_t permissions)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[0] = ADD_PAGE_SYSCALL;
  msg[1] = pID;
  msg[2] = virtual_address;
  msg[3] = permissions;

  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);

  return msg[0];
}

int32_t
MapPageRequest(pID_t src_pID, uint32_t src_virtual_address,
               pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[0] = MAP_PAGE_SYSCALL;
  msg[1] = src_pID;
  msg[2] = src_virtual_address;
  msg[3] = dst_pID;
  msg[4] = dst_virtual_address;
  msg[5] = permissions;

  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);

  return msg[0];
}

int32_t
GrantPageRequest(pID_t src_pID, uint32_t src_virtual_address,
                 pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[0] = GRANT_PAGE_SYSCALL;
  msg[1] = src_pID;
  msg[2] = src_virtual_address;
  msg[3] = dst_pID;
  msg[4] = dst_virtual_address;
  msg[5] = permissions;

  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);

  return msg[0];
}

int32_t
UnmapPageRequest(pID_t pID, uint32_t virtual_address)
{
  uint32_t msg[SERVER_MSG_SIZE];

  msg[0] = UNMAP_PAGE_SYSCALL;
  msg[1] = pID;
  msg[2] = virtual_address;

  Send(server_pID, msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, msg, SERVER_MSG_SIZE, 0);

  return msg[0];
}

/* GetProcess variations. These all call GetProcess().*/

uint32_t
GetProcessProperty(pID_t pID, uint32_t property)
{
  uint32_t result;
  uint32_t properties[2];

  properties[0] = property;

  if ((result = GetProcess(pID, properties)) < 0) {
    kprintf("Get error.");
    return result;
  } else {
    return properties[0];
  }
}

uint32_t
GetNumProcesses(void)
{
  return GetProcessProperty(0, K_NUM_PROCS);
}

int32_t
GetCoord(void)
{
  return GetProcessProperty(0, K_COORD);
}

uint32_t
GetTime(void)
{
  return GetProcessProperty(0, K_TIME);
}

int32_t
GetCurrentPid(void)
{
  return GetProcessProperty(USE_CURRENT_PROCESS, P_PID);
}

// The following GetProcess calls are pID-specific.
uint32_t
GetPrivilege(pID_t pID)
{
  return GetProcessProperty(pID, P_PRIVILEGE);
}

int32_t
GetBasePage(pID_t pID)
{
  return GetProcessProperty(pID, P_BASEPAGE);
}

int32_t
GetStatus(pID_t pID)
{
  return GetProcessProperty(pID, P_STATUS);
}

int32_t
GetReason(pID_t pID)
{
  return GetProcessProperty(pID, P_REASON);
}

int32_t
GetSize(pID_t pID)
{
  return GetProcessProperty(pID, P_SIZE);
}

int32_t
GetTimeRunning(pID_t pID)
{
  return GetProcessProperty(pID, P_RUNNING_TIME);
}

int32_t
GetStartTime(pID_t pID)
{
  return GetProcessProperty(pID, P_START_TIME);
}

int32_t
GetCurrentPriority(pID_t pID)
{
  return GetProcessProperty(pID, P_CURR_PRI);
}

int32_t
GetPriority(pID_t pID)
{
  return GetProcessProperty(USE_CURRENT_PROCESS, P_PRIORITY);
}

int32_t
GetParentPid(int32_t pID)
{
  return GetProcessProperty(pID, P_PARENT);
}

int32_t
GetUserStack(pID_t pID)
{
  return GetProcessProperty(pID, P_USTACK);
}

int32_t
GetKernelStack(pID_t pID)
{
  return GetProcessProperty(pID, P_KSTACK);
}

int32_t
GetMemoryRights(int32_t pID)
{
  return GetProcessProperty(pID, P_MEM_RIGHTS);
}

