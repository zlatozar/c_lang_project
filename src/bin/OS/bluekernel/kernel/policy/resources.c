///////////////////////////////////////////////////////////////////////////////////////////////////
/* Resource Management
	resources.c

	This implements:
	1. Resource Table for tracking which processes have
	access to which resources (interrupts, I/O ports, I/O memory, DMA).
	2. Add/Remove Resources for processes.
	3. Interrupt handling and corresponding Sleep call for user processes.
	4. Exception handling.
	5. Read/Write to I/O ports.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_resources_add Resources Add/Remove Code
// @I web_resources_interrupts Resources Interrupt Code
// @E web_resources_exceptions Resources Exceptions Code
// @A+
// @I+
// @E+

#include "global.h"
#include "mec.h"

///// Resource Management Interfaces //////////////////////////////////////////////////////////////
// @I-
// @E-
int AddResource(pID_t pID, uint32_t resource_type, uint32_t index, uint32_t value);
int RemoveResource(uint32_t resource_type, int32_t index);
int RemoveProcessResources(process_table_t* pID_ptr);
// @A-
// @I+
int Interrupt(int hwintn);
// @I-
// @E+
int Exception(int exc_num, uint32_t exc_msg);
// @E-

///// Resource Management Local Functions /////////////////////////////////////////////////////////

///// External Interfaces /////////////////////////////////////////////////////////////////////////
extern int Schedule(void);
extern int SetProcess(process_table_t* pID_ptr, int property, uint32_t value);
extern int Check(void);
extern int CheckCurrent(process_table_t* pID_ptr);

extern process_table_t* pIDtoPtr(pID_t pID);

extern int UpdateTimers(void);

extern pID_t Send(pID_t receiver_pID, uint32_t *sender_msgB, uint32_t length, uint32_t timeout);
extern int   RemoveMessage(process_table_t* target_ptr);

extern int Error(int code);

///// Local Data //////////////////////////////////////////////////////////////////////////////////

// @A+
///////////////////////////////////////////////////////////////////////////////////////////////////
///// Resource Management Interfaces //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int AddResource(pID_t pID, uint32_t resource_type, uint32_t index, uint32_t value)
{
	running_ptr->last_syscall = resource_type;
	LockBus();
	
	if (Check() < OK)
	{
		UnlockBus();
		return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
	}
	
	switch (resource_type)
	{
		case INT_RESOURCE :
		{
			//int interruptID = index;
			index -= INTERRUPT_OFFSET;
			if (index < 0 || index >= NUM_INTERRUPTS)
			{
				UnlockBus();
				return ERROR_INT_NUM_RANGE;
			}
			if (interruptT[index].pID != 0)
			{
				UnlockBus();
				return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
			}
			interruptT[index].pID = pID;
			UnlockBus();
			return OK;
		}
		case IOPORT_RESOURCE :
		{
			index -= PORT_PID_FIRST;
			if (index >= NUM_IOPORTS)
			{
				UnlockBus();
				return ERROR_PORT_NUM_RANGE;
			}
			if (portT[index].pID != 0)
			{
				UnlockBus();
				return ERROR_CHECK_ADD_RESOURCE_SYSCALL;
			}
			portT[index].port	= value;
			portT[index].pID	= pID;
			UnlockBus();
			return index;
		}
	}
	UnlockBus();
	return ERROR_ADD_RESOURCE_SYSCALL;
}

int RemoveResource(uint32_t resource_type, int32_t index)
{
	LockBus();
	running_ptr->last_syscall = resource_type;
	
	switch (resource_type)
	{
		case INT_RESOURCE :
		{
			index -= INTERRUPT_OFFSET;
			
			if (interruptT[index].pID != current_pID && Check() < OK)
			{
				UnlockBus();
				return ERROR_REMOVE_RESOURCE_CHECK;
			}
			else
			{
				interruptT[index].pID = 0;
			}
			break;
		}
		case IOPORT_RESOURCE :
		{
			index -= PORT_PID_FIRST;
			if (portT[index].pID != current_pID && Check() < OK)
			{
				UnlockBus();
				return ERROR_REMOVE_RESOURCE_CHECK;
			}
			else
			{
				portT[index].pID = 0;
			}
			break;
		}
		case ALL_RESOURCES :
		{
			//use index as pID parameter:
			process_table_t* pID_ptr;
			
			if (index == USE_CURRENT_PROCESS)
			{
				pID_ptr = running_ptr;
			}
			else if (!(pID_ptr = pIDtoPtr(index)))
			{
				UnlockBus();
				return ERROR_PID_PTR;
			}
			RemoveProcessResources(pID_ptr);
			break;
		}
		default :
		{
			UnlockBus();
			return ERROR_REMOVE_RESOURCE_CHECK;
		}
	}
	UnlockBus();
	return OK;
}

int RemoveProcessResources(process_table_t* pID_ptr)
{
	if (CheckCurrent(pID_ptr) < OK)
		return ERROR_REMOVE_PROCESS_RESOURCES_CHECK;
	
	int i=0;
	
	while (i < NUM_IOPORTS)
	{
		if (portT[i].pID == pID_ptr->pID)
		{
			portT[i].port = 0;
			portT[i].pID = 0;
		}
		i++;
	}
	
	i = 0;
	
	while (i < NUM_INTERRUPTS)
	{
		if (interruptT[i].pID == pID_ptr->pID)
		{
			interruptT[i].pID = 0;
		}
		i++;
	}
	return OK;
}
// @A-

///// Interrupts & Exceptions /////////////////////////////////////////////////////////////////////

// @I+
int Interrupt(int hwintn)
{
	#if KERNEL_DEBUG
	/* Hardware interrupt 7 seems to trigger for no good reason. This catch would be better off in
	 * the mechanisms layer.*/
	if (hwintn == HWINT7)
		return OK;
	#endif

	LockBus();
	
	if (hwintn == CLOCK_INTERRUPT)
	{
		ClockHandler(); //re-enables CLOCK_INTERRUPT
		clock_schedule++;
		clock_ticks++;
		
		if (num_sleepers > 0)
			UpdateTimers();
		
		if (clock_ticks >= TICK_FREQ)
		{
			clock_seconds++;
			clock_ticks = 0;
		}
		if (clock_schedule >= SCHEDULE_RATE)
		{
			clock_schedule = 0;
			Schedule();
		}
	}
	else
	{
		// Check if there is a process sleeping to handle interrupt use hwint'n' to look up
		// interrupt table entry 'n'.
		process_table_t* handler_ptr = interruptT[hwintn - INTERRUPT_OFFSET].own_msgQn;
		interruptT[hwintn - INTERRUPT_OFFSET].occurrences++;
		
		
		if (handler_ptr != 0 && handler_ptr != (process_table_t*)hwintn)
		{
			// Found handler waiting for interrupt.
			
			#if KERNEL_DEBUG
			interrupt_count++;
			last_interrupt = hwintn;
			#endif
			
			RemoveMessage(handler_ptr);
			SetProcess(handler_ptr, P_READY, REASON_INTERRUPT_DONE);
		}
		else
		{
			// No handler waiting for interrupt.
			
			#if KERNEL_DEBUG
			wakeup_count++;
			#endif

			interruptT[hwintn-INTERRUPT_OFFSET].wakeups++;
			UnlockBus();
			
			return ERROR_NO_INT_HANDLER;
		}
	}
	
	UnlockBus();
	return OK;
}
// @I-

// @E+
int Exception(int exc_num, uint32_t exc_msg)
{
	pID_t handler_pID;
	pID_t response_pID;
	
	LockBus();
	
	#if VIDEO_DEBUG
		#if KERNEL_DEBUG
			exception_count++;
			if (exception_count < 10)
		#endif

		VideoExceptionOut(exc_msg);

		#if KERNEL_DEBUG
			//else DEBUG;
		#endif
	#endif //VIDEO_DEBUG

	#if KERNEL_DEBUG
		if (current_pID == KERNEL_PID)
			while (1)
				Error(1);
	#endif
	
	num_errors++;
	
	// Check if there is a process sleeping to handle exception
	if (exc_num == PAGER_EXCEPTION)
	{
		handler_pID = running_ptr->pager_pID;
	}
	else
	{
		handler_pID = running_ptr->excepter_pID;
	}
	
	if (handler_pID !=0)
	{
		UnlockBus();
		response_pID = Send(handler_pID, (uint32_t*)exc_msg, EXCEPTION_STACK_SIZE, TIMEOUT_EXCEPTION);
		LockBus();
		
		if (response_pID != handler_pID || running_ptr->reason != REASON_EXCEPTION_DONE)
		{
			/* If handler hasn't already fixed the exception before Send() returns..., or msg does
			 * not arrive successfully.*/
			
			SetProcess(running_ptr, P_BLOCKED, REASON_EXCEPTION);
			Schedule();
		}
		else
		{
			UnlockBus();
		}
		return OK;
	}
	else
	{
		SetProcess(running_ptr, P_BLOCKED, ERROR_NO_EXC_HANDLER);
		Schedule();
		return ERROR_NO_EXC_HANDLER;
	}
}
// @E-

/* END OF FILE */
