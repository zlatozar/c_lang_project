///////////////////////////////////////////////////////////////////////////////////////////////////
/* Resource Mechanisms for IA32/PC
	resources_pc.c
	This file does:
	1. initialisation of interrupts.
	2. clock handler

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @I web_mec_pc_resources_interrupts Resource Mechanism Interrupts Code
// @P web_mec_pc_resources_ports Resource Mechanism Ports Code
// @C web_mec_pc_resources_clock Resource Mechanism Clock Handler Code
// @I+
// @P+
// @C+

#include "resources_pc.h"
#include "../../policy/global.h"

///// Resource Mechanism Interfaces ///////////////////////////////////////////////////////////////
// @I-
// @C-

int			WritePort(uint32_t resource_num, uint32_t *data, int32_t port_type);
int			ReadPort(uint32_t resource_num, uint32_t *data, int32_t port_type);
// @P-

// @I+
int			EnableInterrupt(int hwintn);
uint8_t		DisableInterrupt(int hwintn);
// @I-
// @C+
int			ClockHandler(void);
// @C-
uint32_t	GetMemorySize(void);

///// Resource Mechanism Local Functions //////////////////////////////////////////////////////////

#if KERNEL_DEBUG
int		InterruptMec(int hwintn);
int		DebugCheckClock(void);
#endif

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern uint8_t  In8(uint32_t port);
extern uint32_t Out8(uint32_t port, uint8_t data);
extern uint16_t In16(uint32_t port);
extern uint32_t Out16(uint32_t port, uint16_t data);
extern uint32_t In32(uint32_t port);
extern uint32_t Out32(uint32_t port, uint32_t data);
extern void		ReloadGDT(void);
extern void		LoadTaskRegister(uint16_t tss_selector);

// hardware interrupts.
extern int		HwInt0(), HwInt1(), HwInt2(), HwInt3(), HwInt4(), HwInt5(), HwInt6(), HwInt7(),
				HwInt8(), HwInt9(), HwInt10(), HwInt11(), HwInt12(), HwInt13(), HwInt14(),
				HwInt15();

// exceptions.
extern int		DivideErrorExc(), SingleStepExc(), NmiExc(), BpExc(), OverflowExc(),
				BoundsCheckExc(), InvalidOpcodeExc(), CoCpuExc(), DoubleFaultExc(),
				CoCpuOverrunExc(), InvalidTssExc(), SegNotPresentExc(), StackExc(),
				GenProtectionExc(), PageFaultExc(), CoCpuErrorExc();

///// Local Data //////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Resource Mechanism Interfaces ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///// Write Port I/O //////////////////////////////////////////////////////////////////////////////

// @P+
int WritePort(uint32_t resource_num, uint32_t *data, int32_t port_type)
{
	running_ptr->last_syscall	= port_type;
	resource_num				-= PORT_PID_FIRST; // port index offset
	
	if (portT[resource_num].pID != current_pID)
		return ERROR_WRITE;
	
	switch (port_type)
	{
		case DATA8 :
		{
			Out8(portT[resource_num].port, (uint8_t)*data);
			break;
		}
		case DATA16 :
		{
			Out16(portT[resource_num].port, (uint16_t)*data);
			break;
		}
		case DATA32 :
		{
			Out32(portT[resource_num].port, (uint32_t)*data);
			break;
		}
		default :
		{
			return ERROR_WRITE;
		}
	}
	return OK;
}

int ReadPort(uint32_t resource_num, uint32_t *data, int32_t port_type)
{
	running_ptr->last_syscall	= port_type;
	resource_num				-= PORT_PID_FIRST; // port index offset
	
	if (portT[resource_num].pID != current_pID)
		return ERROR_READ;
	
	switch (port_type)
	{
		case DATA8 :
		{
			uint8_t *temp_ptr	= (uint8_t*)data;
			*temp_ptr			= In8(portT[resource_num].port);
			break;
		}
		case DATA16 :
		{
			uint16_t *temp_ptr	= (uint16_t*)data;
			*temp_ptr		= In16(portT[resource_num].port);
			break;
		}
		case DATA32 :
		{
			data[0]			= In32(portT[resource_num].port);
			break;
		}
		default :
		{
			return ERROR_READ;
		}
	}
	return OK;
}
// @P-

// @I+
int EnableInterrupt(int hwintn)
{
	if (hwintn < HWINT_FIRST || hwintn > HWINT_LAST)
		return ERROR_INT_NUM_RANGE;
	
	uint8_t	interrupt_mask, check; // 'check' is really just for debugging.
	int		int_offset, int_port;
	
	if (hwintn < HWINT8) // master PIC
	{
		int_offset	= HWINT0;
		int_port	= PIC1_DATA;
	}
	else // slave PIC
	{
		int_offset	= HWINT8;
		int_port	= PIC2_DATA;
	}
	interrupt_mask	= In8(int_port);
	check			= interrupt_mask & 0x01;
	
	if (check != 0)
		check = In8(int_port);
	
	interrupt_mask &= ~(1<<(hwintn - int_offset)); // re-enable IRQn
	
	Out8(int_port, interrupt_mask); //enable interrupt in PIC
	
	return OK;
}

uint8_t DisableInterrupt(int hwintn)
{
	if (hwintn < HWINT_FIRST || hwintn > HWINT_LAST)
		return ERROR_INT_NUM_RANGE;
	
	uint8_t	interrupt_mask, check;
	int		int_offset, int_port;
	
	if (hwintn < HWINT8) // master PIC
	{
		int_offset	= HWINT0;
		int_port	= PIC1_DATA;
	}
	else // slave PIC
	{
		int_offset	= HWINT8;
		int_port	= PIC2_DATA;
	}
	
	interrupt_mask	= In8(int_port);
	check			= interrupt_mask;
	interrupt_mask	&= (1<<(hwintn - int_offset)); // disable IRQn
	
	Out8(int_port, interrupt_mask);
	
	return check;
}
// @I-

// @C+
int ClockHandler(void)
{
	uint8_t interrupt_mask;
	
	interrupt_mask	= In8(PIC1_DATA);
	interrupt_mask	&= ~(1 << (CLOCK_INTERRUPT-INTERRUPT_OFFSET)) ; // re-enable IRQ0
	
	Out8(PIC1_DATA, interrupt_mask);
	
	return OK;
}

uint16_t ClockCounter(void)
{
	//mov al, 00000000b    ; al = channel in bits 6 and 7, remaining bits clear
	//out 0x43, al         ; Send the latch command
	Out8(TIMER_MODE, 0); // channel 0.

	//in al, 0x40          ; al = low byte of count
	uint8_t low_count = In8(TIMER0);

	//mov ah, al           ; ah = low byte of count

	//in al, 0x40          ; al = high byte of count
	uint8_t high_count = In8(TIMER0);

	//rol ax, 8            ; al = low byte, ah = high byte (ax = current count)

	uint16_t counter = (high_count << 8) | low_count;
	
	return counter;
}
// @C-

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Resource Mechanism Local Functions //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#if KERNEL_DEBUG
int InterruptMec(int hwintn)
{
	uint8_t check	= DisableInterrupt(hwintn);
	check			&= 0x01;
	
	if (check != 0)
		check = In8(PIC1_DATA);
	
	Out8(PIC1_COMMAND, PIC_EOI); //ack int code for PIC, clears highest IRQ from ISR.
	
	return OK;
}

int DebugCheckClock(void)
{
	uint8_t interrupt_mask, check;
	
	interrupt_mask	= In8(PIC1_DATA);
	check			= interrupt_mask & 0x01;
	
	if (check != 0)
		check = In8(PIC1_DATA);
	
	return OK;
}
#endif

/* END OF FILE */
