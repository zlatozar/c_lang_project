///////////////////////////////////////////////////////////////////////////////////////////////////
/* Kernel global data definitions
  global.h

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdint.h>
#include "kernel.h"

/* this is required for PROCESS_MECHANISMS in the process table definition. */
#include "../mechanism/pc/kernel_ia32.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Kernel Global Data //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///// Process Data ////////////////////////////////////////////////////////////////////////////////

typedef struct process_table_t {
  pID_t   pID, parent_pID;
  struct process_table_t* child, *sibling;
  struct process_table_t* next_thread, *thread_owner;
  struct process_table_t* next, *prev;

  uint8_t   status;
  uint8_t priority; //bits0-3 priority, bit 4 addr space owner
  uint8_t current_priority; // not used.
  uint8_t privilege;

  int16_t   reason; //for being blocked, or made ready
  uint8_t times_scheduled;
  // next 1 byte slot is not used.

  uint32_t  time_started;
  uint32_t  time_running;
  uint32_t  time_blocked;
  uint32_t  blocked_ticks;
  uint32_t  timer;

  // Memory Settings //
  uint32_t  first_instruction;
  uint32_t  user_stack_addr;
  uint32_t  kernel_stack_addr;
  uint32_t  base_page;
  uint32_t  size;
  int32_t   page_maps;

  // Handlers //
  pID_t   pager_pID;
  pID_t   excepter_pID;
  uint32_t  last_syscall;

  // Messages //
  pID_t   prev_msg_target_pID;
  int32_t   msg_range_length;
  struct process_table_t* own_msgQn, *own_msgQp, *wait_msgQn, *wait_msgQp;
  uint32_t  msg[MSG_MAX_LENGTH];

  uint32_t  mechanisms[PROCESS_MECHANISMS];

#if KERNEL_DEBUG // Padding to make debugging easier
#define NUM_DUMMIES 0
  uint32_t  dummy[NUM_DUMMIES];
#endif
} process_table_t;

process_table_t* processT, *first_process_ptr, *last_process_ptr, *readyQ, *ready_fastQ, *blockedQ,
                 *idle_ptr, *running_ptr;

uint32_t  pID_transparent_mask;
pID_t   current_pID;
uint32_t  num_schedules;
uint32_t  num_processes;
pID_t   max_pID;

///// Resource Data ///////////////////////////////////////////////////////////////////////////////

uint32_t  num_sleepers;
uint32_t  num_errors;
uint32_t  num_check_failures;
int32_t   coord_set;

uint32_t  clock_seconds;
uint32_t  clock_schedule;
uint32_t  clock_ticks;
uint32_t  prev_ticks;
uint32_t  prev_seconds;

typedef struct interrupt_table_t {
  uint8_t   int_num; //debugging.
  pID_t   pID;
  struct process_table_t* own_msgQn, *own_msgQp; //waiting list of processes
  uint32_t  wakeups;
  uint32_t  occurrences;
  uint32_t  dummy[2];
} interrupt_table_t;
interrupt_table_t* interruptT;

typedef struct port_table_t {
  uint32_t  port;
  pID_t   pID;
} port_table_t;
port_table_t*  portT;

///// Memory Data /////////////////////////////////////////////////////////////////////////////////

uint32_t  kernel_base_page;

typedef struct page_owners_t {
  int32_t   next_page;
  int32_t   page_maps;
} page_owners_t;
page_owners_t* page_ownersT; // its size is set by num_pages

typedef struct page_maps_t {
  uint32_t  map_num; //debugging
  struct process_table_t* pID_ptr;
  int32_t   physical_page;
  uint32_t  dummy[2]; //debugging alignment.
  int32_t   virtual_page;
  pID_t   next_pID; // next page map for the same process.
  uint32_t  next_phys; // next page map for the same phys page.
} page_maps_t;
page_maps_t* page_mapsT; //[NUM_MAPS]

uint32_t  next_free_page;
uint32_t  num_free_pages;
int32_t   next_free_map;
uint32_t  num_free_maps;

uint32_t  memory_size;
uint32_t  num_pages;

unsigned* kernel_heap_next;

///// Debugging Data and Definitions //////////////////////////////////////////////////////////////

#if KERNEL_DEBUG
#define SCHEDULE_LOG_MAX 500
int32_t*   schedule_log;
int     sch_log_index;
int     debug_count;
#define MSG_LOG_MAX 500
int32_t*   message_log;
int     msg_log_index;
int     exception_count;
int     interrupt_count, last_interrupt, wakeup_count;
int     exception_log[EXCEPTION_LAST - EXCEPTION_FIRST];

typedef struct debug_t {
  pID_t   current_pID;
  uint32_t  tss_selector; //could divide by 8 to get index into GDT.
  struct process_table_t* readyQ, *own_msgQn, *own_msgQp, *wait_msgQn, *wait_msgQp;
  //all of current interrupt fields.
} debug_t;
debug_t debugT;

uint32_t  wakeup7; //debug for stray interrupts on hwint#7.

#endif

#endif

/* END OF FILE */
