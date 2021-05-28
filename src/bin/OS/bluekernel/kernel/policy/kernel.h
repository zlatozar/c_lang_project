///////////////////////////////////////////////////////////////////////////////////////////////////
/* Kernel Header
	kernel.h

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>
#include "../mechanism/pc/resources_pc.h"
#include "../mechanism/pc/boot/bootdata.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Kernel Policy Definitions ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/* some libraries that include kernel.h define NULL themselves, but we need it in the kernel.*/
#ifndef NULL
#define NULL 0
#endif

// process ID type.
typedef int32_t				pID_t;

///// Process Definitions /////////////////////////////////////////////////////////////////////////

#define MAX_PROCESSES		12
#define IDLE_PID			(0 + 1)
// first procesT slot with min version number.

#define KERNEL_PID			0x10
// this needs to be clear of proper pIDs.

#define USE_CURRENT_PROCESS	0x11
#define USE_COORDINATOR		0x12
#define ANY					IDLE_PID

#define kernel_ptr			(struct process_table_t* )0x12345678
#define MIN_PID				IDLE_PID
#define MAX_PID				(MAX_PROCESSES << VERSION_SHIFT) - 1
#define THREAD_FIELD		0x10
// thread bit is part of priority parameter.

#define VERSION_SHIFT		12
#define VERSION_MASK		((1 << (*boot_version_bits)) - 1)
// This comes to 0xFFF

#define PID_MASK			-1 ^ VERSION_MASK

#define INTERRUPT_OFFSET	0x20
/* ((MAX_PROCESSES) << VERSION_SHIFT) // use this if placing interrupt numbers after last possible
 * process ID. Exceptions and Ports follow on from Interrupts.*/

#define MAX_INTERRUPT		HWINT_LAST
#define MAX_EXCEPTION		EXCEPTION_LAST
#define MAX_PORT			PORT_PID_LAST

// Process status values:
#define EMPTY	2
#define READY	3
#define RUNNING	4
#define BLOCKED	5
#define PENDING	6

// Process priority // anywhere between 1 and 15
#define NO_PRIORITY		0 // reserved for Idle process.
#define NORMAL_PRIORITY	4
#define FAST_PRIORITY	8

// Process privileges
#define USER_PRIVILEGE		0
#define COORD_PRIVILEGE		1
#define ADDR_SPACE_OWNER	2

////////////// Property/Field Codes ///////////////////////////////////////////////////////////////
// Kernel data/field indicators
#define K_TIME			0x10
#define K_NUM_PROCS		0x11
#define K_COORD			0x12
#define K_MAX_PROCESSES	0x13

#define K_LAST_FIELD	0x80

// Process property indicators, eg to tell SetProcess which property to set.
#define P_PRIORITY		0x20
#define P_PID			0x21
#define P_BASEPAGE		0x22
#define P_STATUS		0x23
#define P_REASON		0x24
#define P_SIZE			0x25
#define P_RUNNING_TIME	0x26
#define P_CURR_PRI		0x27
#define P_START_TIME	0x28
#define P_BLOCKED		0x29
#define P_READY			0x2A
#define P_PRIVILEGE		0x2B
#define P_PARENT		0x2C
#define P_MEM_RIGHTS	0x2D
#define P_USTACK		0x2E
#define P_KSTACK		0x2F

// Reasons for a process's status (reason field can also be set to error code).
#define REASON_PENDING					0x11
#define REASON_NEW						0x12
#define REASON_REMOVED					0x13
#define REASON_RUNNING					0x14
#define REASON_RUNNING_DONE				0x15
#define REASON_BLOCKED					0x16

#define REASON_SEND						0x21
#define REASON_SEND_DONE				0x22
#define REASON_SEND_TIMEOUT				0x23
#define REASON_RECEIVE					0x31
#define REASON_RECEIVE_DONE				0x32
#define REASON_RECEIVER_TIMEOUT			0x33
#define REASON_MESSAGE_TARGET_REMOVED	0x3F
#define REASON_INTERRUPT				0x40
#define REASON_INTERRUPT_DONE			0x41
#define REASON_EXCEPTION				0x42
#define REASON_EXCEPTION_DONE			0x43

// provide space for extra definitions:
#define REASON_KERNEL_LAST				0x7F

///// Error Codes /////////////////////////////////////////////////////////////////////////////////

#define OK										1
#define ERROR									-1
#define ERROR_PROCESS_TABLE_FULL 				-2
#define ERROR_ADD_PROCESS		 				-3
#define ERROR_CHECK_ADD_PROCESS					-4
#define ERROR_ADD_MEMORY						-5
#define ERROR_ADD_PROCESS_MEC    				-6
#define ERROR_NO_COORDINATOR					-7
#define ERROR_CHECK_FAILED						-8
#define ERROR_GET_PROCESS						-9
#define ERROR_PID_PTR							-0x0A
#define ERROR_GET_PROCESS_PROPERTY				-0x0B
#define ERROR_SET_PROCESS						-0x0C
#define ERROR_SET_PROCESS_PROPERTY				-0x0D
#define ERROR_SET_BLOCKED						-0x0E

#define ERROR_CHECK_ADD_RESOURCE_SYSCALL		-0x0F
#define ERROR_INT_NUM_RANGE						-0x10
#define ERROR_PORT_NUM_RANGE					-0x11
#define ERROR_ADD_RESOURCE_SYSCALL				-0x12
#define ERROR_REMOVE_RESOURCE_CHECK				-0x13
#define ERROR_REMOVE_RESOURCE					-0x14
#define ERROR_REMOVE_PROCESS_RESOURCES_CHECK	-0x15
#define ERROR_READ								-0x16
#define ERROR_WRITE								-0x17
#define ERROR_SEND_FAILED						-0x18
#define ERROR_RECEIVE_FAILED					-0x19
#define ERROR_DEADLOCK							-0x1A

#define ERROR_SEND_FAILED_NO_TARGET				-0x1B
#define ERROR_RECEIVE_FAILED_NO_TARGET			-0x1C
#define ERROR_NO_INT_HANDLER					-0x1D
#define ERROR_NOT_INTERRUPT_HANDLER				-0x1E
#define ERROR_NO_EXC_HANDLER					-0x1F

#define ERROR_NO_FREE_PAGES						-0x20
#define ERROR_NO_PHYSICAL_PAGE					-0x21
#define ERROR_PAGE_MAP_PROCESS_NOT_FOUND		-0x22
#define ERROR_GRANT_PAGE_SRC_PID 				-0x23
#define ERROR_SRC_NOT_PAGE_OWNER 				-0x24
#define ERROR_GRANT_PAGE_DST_PID 				-0x25
#define ERROR_GRANT_UNMAP_NOT_OWNER 			-0x26
#define ERROR_CHECK_PAGE_OWNER 					-0x27
#define ERROR_ADD_KERNEL_PAGE 					-0x28
#define ERROR_PAGE_MAPS_FULL 					-0x29
#define ERROR_PHYS_PAGE_NUM						-0x2A
#define ERROR_ADDR_RANGE						-0x2B
#define ERROR_GRANT_PAGE						-0x2C
#define ERROR_PAGE_ALREADY_MAPPED				-0x2D
#define ERROR_MAP_NUM							-0x2E
#define ERROR_NO_FREE_MAPS						-0x2F

/* reserve space for extra kernel definitions, and allow the user space to define extra
 * errors from this point onwards:*/
#define ERROR_KERNEL_LAST						-0x7F 

///// Messaging Defintions ////////////////////////////////////////////////////////////////////////

#define MSG_MAX_LENGTH				10 
// this is 10 dwords for testing

#define MSG_TYPE					0
// field offset in msg buffer

#define any_ptr						idle_ptr
/* the idle process slot is used for the 'any' messages; they are identical - it just reads better
 * in message code to see 'any' when that is the type of target.*/

// length/target range field. Low 4 bits = length.
#define MSG_LENGTH_MASK		0x0F
#define MSG_RANGE_MASK		0x000FFFF0
#define TTHREAD				BIT4
#define RTHREAD				BIT5
#define TSELF				BIT6
#define TCHILD				BIT7
#define TPARENT				BIT8
#define RSELF				BIT9
#define RCHILD				BIT10
#define RPARENT				BIT11
#define TSELF_ANSWERED		BIT12
#define TCHILD_ANSWERED		BIT13
#define TPARENT_ANSWERED	BIT14
#define TTHREAD_ANSWERED	BIT15
#define TANY_ANSWERED		BIT16

// System Calls (Message Types) ///////////////////////////////////////////////////////////////////
#define FIRST_SYSCALL			0x10

// process
#define SCHEDULE_SYSCALL		0x10
#define ADD_PROCESS_SYSCALL		0x11
#define REMOVE_PROCESS_SYSCALL	0x12
#define GET_PROCESS_SYSCALL		0x13
#define SET_PROCESS_SYSCALL		0x14

// message
#define SEND_SYSCALL			0x15
#define RECEIVE_SYSCALL			0x16

// resource
#define ADD_RESOURCE_SYSCALL	0x17
#define REMOVE_RESOURCE_SYSCALL	0x18
#define INTERRUPT_SYSCALL		0x19
#define EXCEPTION_SYSCALL		0x1A

// memory
#define ADD_PAGE_SYSCALL		0x1B
#define MAP_PAGE_SYSCALL		0x1C
#define GRANT_PAGE_SYSCALL		0x1D
#define UNMAP_PAGE_SYSCALL		0x1E

#define LAST_SYSCALL			UNMAP_PAGE_SYSCALL

///// Resource Definitions ////////////////////////////////////////////////////////////////////////

#define NUM_IOPORTS				0x50
#define NUM_INTERRUPTS			(HWINT_LAST - HWINT_FIRST + 1)
#define NUM_EXCEPTIONS			(EXCEPTION_LAST - EXCEPTION_FIRST + 1)

#define TIMEOUT_INTERRUPT		0xFFF
#define TIMEOUT_EXCEPTION		0xFFF

#define ALL_RESOURCES			0x32
#define INT_RESOURCE 			0x33
#define IOPORT_RESOURCE 		0x34
#define DMA_RESOURCE 			0x35

// Ports
#define PORT_PID				(MAX_EXCEPTION + 1)
#define PORT_PID_FIRST			PORT_PID
#define PORT_PID_LAST			(PORT_PID + NUM_IOPORTS)

///// Memory Definitions //////////////////////////////////////////////////////////////////////////

#define READ			0x10000000
#define WRITE			0x20000000
#define EXECUTE			0x40000000
#define KERNEL_PAGE		0x80000000u
#define PAGE_PRESENT	0x01000000
#define PAGE_DIRTY		0x02000000
#define PAGE_ACCESSED	0x04000000

#define USER_RWX		(READ + WRITE + EXECUTE)
#define USER_RW			(READ + WRITE)
#define USER_R			(READ)
#define USER_R_X		(READ + EXECUTE)
#define KERNEL_RWX		(KERNEL_PAGE + READ + WRITE + EXECUTE)
#define KERNEL_RW		(KERNEL_PAGE + READ + WRITE)
#define KERNEL_R		(KERNEL_PAGE + READ)
#define KERNEL_R_X		(KERNEL_PAGE + READ + EXECUTE)

#define KSTACK_SIZE		256
#define NUM_MAPS		64
#define EMPTY_PAGE_MAP	-8
#define NULL_PAGE		-1

#define GB				0x40000000u
#define MB				0x100000u
#define KB				0x0400u
// the 'u' suffix forces constants to be unsigned.

// Video Controls for video debug
#define VIDEO_DEBUG		1
#define V_NORMAL		0
#define V_EXCEPTION		1

#define KERNEL_DEBUG	1

#endif

/* END OF FILE */
