///////////////////////////////////////////////////////////////////////////////////////////////////
/* Header for System Call Extensions
	syscallext.h

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SYSCALLEXT_H
#define __SYSCALLEXT_H

#include "stdlib.h"
#include "syscall.h"
#include "../../kernel/policy/kernel.h"
#include "../../server/server.h"

int			SetProcessRequest(int32_t pID, int property, uint32_t value);
int32_t		GetCoordinator(void);
int			SetAsCoordinator(void);
void		GetSystemInfo(void);

int32_t		AddPageRequest(pID_t pID, uint32_t virtual_address, uint32_t permissions);
int32_t		MapPageRequest(pID_t src_pID, uint32_t src_virtual_address,
						   pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions);
int32_t		GrantPageRequest(pID_t src_pID, uint32_t src_virt_addr,
							 pID_t dst_pID, uint32_t dst_virt_addr, uint32_t permissions);
int32_t		UnmapPageRequest(pID_t pID, uint32_t virtual_address);

/* GetProcess variations */
uint32_t	GetNumProcesses(void);
uint32_t	GetTime(void);
int32_t		GetCurrentPid(void);

uint32_t	GetPrivilege(pID_t pID);
int32_t		GetBasePage(pID_t pID);
int32_t		GetBasePage(pID_t pID);
int32_t		GetReason(pID_t pID);
int32_t		GetSize(pID_t pID);
int32_t		GetTimeRunning(pID_t pID);
int32_t		GetStartTime(pID_t pID);
int32_t		GetCurrentPriority(pID_t pID);
int32_t		GetPriority(pID_t pID);
int32_t		GetParentPid(pID_t pID);
int32_t		GetUserStack(pID_t pID);
int32_t		GetKernelStack(pID_t pID);
int32_t		GetMemoryRights(pID_t pID);

#endif

/* END OF FILE */
