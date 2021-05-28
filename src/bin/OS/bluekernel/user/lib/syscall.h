///////////////////////////////////////////////////////////////////////////////////////////////////
/* Header for the System Call interfaces
  syscall.h

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SYSCALL_H
#define __SYSCALL_H

#include "../../kernel/policy/kernel.h"
#include "../../kernel/mechanism/pc/resources_pc.h"

pID_t   Send(pID_t receiver_pID, uint32_t* sender_msgB, uint32_t length, uint32_t timeout);
pID_t   Receive(pID_t sender_pID, uint32_t* receiver_msgB, uint32_t length, uint32_t timeout);

pID_t   Sleep(pID_t sender_pID, uint32_t* receiver_msgB, uint32_t length, uint32_t timeout);
// this maps straight on to Receive function.

pID_t   Timer(uint32_t timeout);
// this maps on to Receive function.

int     Schedule(void);
pID_t   AddProcess(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
                   uint32_t user_stack_addr, uint32_t kernel_stack_addr,
                   pID_t excepter_pID, pID_t pager_pID);
int32_t   RemoveProcess(pID_t pID);
uint32_t  GetProcess(pID_t pID, uint32_t* properties);
int     SetProcess(pID_t pID, int property, uint32_t value);

int     AddResource(pID_t pID, uint32_t resource_type, uint32_t index, uint32_t value);
int     RemoveResource(uint32_t resource_type, int32_t index);
int     Read8(uint32_t portID, void* data);
int     Write8(uint32_t portID, uint8_t data);
int     Read16(uint32_t portID, void* data);
int     Write16(uint32_t portID, uint16_t data);
int     Read32(uint32_t portID, void* data);
int     Write32(uint32_t portID, uint32_t data);

int32_t   AddPage(pID_t pID, uint32_t virtual_address, uint32_t permissions);
int32_t   MapPage(pID_t src_pID, uint32_t src_virtual_address,
                  pID_t dst_pID, uint32_t dst_virtual_address, uint32_t permissions);
int32_t   GrantPage(pID_t src_pID, uint32_t src_virtual_address,
                    pID_t dst_pID, uint32_t dst_virtual_address,
                    uint32_t permissions);
int32_t   UnmapPage(pID_t pID, uint32_t virtual_address);

#endif

/* END OF FILE */
