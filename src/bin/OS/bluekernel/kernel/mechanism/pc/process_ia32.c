///////////////////////////////////////////////////////////////////////////////////////////////////
/* Process Mechanisms for IA-32 and PC
  process_ia32.c

  This file contains the i386-specific table code, such as creating GDT, IDT, LDTs, TSSs and the
  various types of descriptors that go in these tables. There are also generic functions to
  coordinate the creation of these tables, such as CreateTask, which creates a TSS and LDT for a
  new task, as well as the GDT entries.

  Paul Cuttler 2012 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_mec_pc_process_add Process Mechanisms Add/Remove Code
// @S web_mec_pc_process_switch Process Switching Mechanisms Code
// @M web_mec_pc_memory_segments Process Mechanisms Add Code
// @A+
// @S+
// @M+

#include "kernel_ia32.h"
#include "../../policy/global.h"

///// Process Mechanism Interfaces ////////////////////////////////////////////////////////////////
// @M-
void    SwitchProcessMec(process_table_t* pID_ptr);
// @M+
int     AddProcessMec(process_table_t* pID_ptr);
// @M-
uint32_t  RemoveProcessMec(process_table_t* pID_ptr);

///// Process Mechanism Local Functions ///////////////////////////////////////////////////////////

///// External Interfaces /////////////////////////////////////////////////////////////////////////

///// Local Data //////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Process Mechanism Interfaces ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// @A-

/* SwitchProcessMec() switches CPU control to the newly scheduled process. The first two mechanism
 * slots contain the 48-bit TSS selector that points to the task state segment for the new process.
 */
void
SwitchProcessMec(process_table_t* pID_ptr)
{
  asm(
    "lea %0,%%eax\n\t"
    "ljmp *(%%eax)\n\t"
    :
    : "m"(pID_ptr->mechanisms[0])
    : "%eax"
  );
}
// @S-

// @A+
// @M+
int
AddProcessMec(process_table_t* pID_ptr)
{
  uint32_t* addr;
  addr = &pID_ptr->mechanisms[2]; // skip over the TSS selector.

  for (int j = 0; j < PROCESS_MECHANISMS - 2; j++) {
    *addr = 0; //back_link
    addr++;
  }
  addr  = &pID_ptr->mechanisms[3];
  *addr = pID_ptr->kernel_stack_addr; //ESP0_value;
  addr++;

  *addr = DSk_SEL; // SS0_value;
  addr  += 5; // skip ESP1, SS1, ESP2, SS2

  *addr = pID_ptr->base_page << 12; //PDBR;
  addr++;

  *addr = pID_ptr->first_instruction; //EIP_value;
  addr++;

  *addr = 0x00000200; //EFLAGS, IF set.
  addr  += 5; // skip EAX, ECX, EDX, EBX

  *addr = pID_ptr->user_stack_addr; //ESP_value;
  addr  += 4; // skip EBP, ESI, EDI.

  *addr = DSu_SEL; //ES_value
  addr++;

  *addr = CSu_SEL; //CS_value;
  addr++;

  *addr = DSu_SEL; //SS_value;
  addr++;

  *addr = DSu_SEL; //DS_value;
  addr  += 3;

  *addr = LDT_SEL;
  addr++;

  *addr = (TSS_SIZE - 1) << 16; // bitmap offset | 0 | Debug Trap Bit.

  return OK;
}
// @M-

uint32_t
RemoveProcessMec(process_table_t* pID_ptr)
{
  return OK;
}
// @A-

/* END OF FILE */
