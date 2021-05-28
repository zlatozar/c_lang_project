///////////////////////////////////////////////////////////////////////////////////////////////////
/* Process Management
  process.c

  This implements the process table, scheduling, process creation &
  destruction, and the setting and getting of the values of a process's
  properties.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_process_add Process Add/Remove Code
// @B web_process_schedule Process Scheduling Code
// @G web_process_get Process Get/Set Code
// @C web_process_check Process Permissions Code

// @A+
// @B+
// @G+
// @C+

#include "global.h"
#include "mec.h"

///// Process Management Interfaces ///////////////////////////////////////////////////////////////
// @A-
// @G-
// @C-
int   ScheduleCall(void);
int   Schedule(void);
// @B-
// @A+
int32_t   AddProcessCall(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
                         uint32_t user_stack_addr, uint32_t kernel_stack_addr,
                         pID_t excepter_pID, pID_t pager_pID);
pID_t   AddProcess(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
                   uint32_t user_stack_addr, uint32_t kernel_stack_addr,
                   pID_t excepter_pID, pID_t pager_pID);
int32_t   RemoveProcessCall(pID_t pID);
int32_t   RemoveProcess(process_table_t* pID_ptr);
int     RemoveChild(process_table_t* pID_ptr);
// @A-
// @C+
int     Check(void);
int     CheckCurrent(process_table_t* pID_ptr);
// @G+
uint32_t  GetProcessCall(pID_t pID, uint32_t* properties);
uint32_t  GetProcess(process_table_t* pID_ptr, uint32_t* properties);
// @C-
int     SetProcessCall(pID_t pID, int property, uint32_t value);
int     SetProcess(process_table_t* pID_ptr, int property, uint32_t value);
// @G-
// @C+
process_table_t* pIDtoPtr(pID_t pID);
// @C-
// @G+
///// Process Management Local Functions //////////////////////////////////////////////////////////

int AddQueue(process_table_t * *queue, process_table_t* pID_ptr);
int RemoveQueue(process_table_t * *queue, process_table_t* pID_ptr);
// @G-

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern int    RemoveProcessMessages(process_table_t* pID_ptr);

extern int32_t  AddMemory(process_table_t* pID_ptr);
extern uint32_t RemoveMemory(process_table_t* pID_ptr);

extern int32_t  AddPage(process_table_t* pID_ptr, uint32_t virtual_address, uint32_t permissions);
extern int32_t  UnmapPage(process_table_t* pID_ptr, uint32_t virtual_address);

extern int    RemoveProcessResources(process_table_t* pID_ptr);


///// Local Data //////////////////////////////////////////////////////////////////////////////////
// @G+
// @C+
///////////////////////////////////////////////////////////////////////////////////////////////////
///// Process Management Functions ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// @G-
// @C-
// @B+
int
ScheduleCall(void)
{
  LockBus();
  Schedule();
  UnlockBus();
  return OK;
}

int
Schedule(void)
{
  num_schedules++;
  running_ptr->time_running += (clock_ticks - prev_ticks
                                + (clock_seconds - prev_seconds) * TICK_FREQ);

  prev_ticks          = clock_ticks;
  prev_seconds        = clock_seconds;

#if VIDEO_DEBUG
  VideoProcessUpdate();
#endif

  int different_process_selected = 0;

  if (ready_fastQ != NULL) {
    if (running_ptr->status == RUNNING)
    { running_ptr->status = READY; }

    if (running_ptr != ready_fastQ->next)
    { different_process_selected = 1; }

    ready_fastQ = ready_fastQ->next;
    running_ptr = ready_fastQ;
  } else if (readyQ != NULL) {
    if (running_ptr->status == RUNNING)
    { running_ptr->status = READY; }

    if (running_ptr != readyQ->next)
    { different_process_selected = 1; }

    readyQ    = readyQ->next;
    running_ptr = readyQ;
  } else {
    if (running_ptr != idle_ptr) {
      different_process_selected  = 1;
      running_ptr         = idle_ptr;
    }
  }

#if KERNEL_DEBUG
  if (sch_log_index >= SCHEDULE_LOG_MAX)
  { sch_log_index = 0; }

  schedule_log[sch_log_index++] = current_pID;
  debugT.current_pID        = current_pID;
  debugT.readyQ         = readyQ;

  // copy some data values to spare locations in Idle process table entry.
  idle_ptr->msg[0] = current_pID;
  idle_ptr->msg[1] = num_schedules;
  idle_ptr->msg[2] = num_processes;
  idle_ptr->msg[3] = num_sleepers;
  idle_ptr->msg[4] = (uint32_t)readyQ;
  idle_ptr->msg[5] = (uint32_t)ready_fastQ;
  idle_ptr->msg[6] = (uint32_t)blockedQ;
  idle_ptr->msg[7] = (uint32_t)running_ptr;
#endif

  if (different_process_selected) {
    running_ptr->status = RUNNING;
    current_pID     = running_ptr->pID;

    if (running_ptr->times_scheduled >= 0xFF)
    { running_ptr->times_scheduled = 0; }
    else
    { running_ptr->times_scheduled++; }

    clock_schedule = 0; // give a new process a full quantum.

    SwitchProcessMec(running_ptr);
  }
  return OK;
}
// @B-
// @C+
pID_t
AddProcessCall(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
               uint32_t user_stack_addr, uint32_t kernel_stack_addr,
               pID_t excepter_pID, pID_t pager_pID)
{
  LockBus();

  if (Check() < OK) {
    UnlockBus();
    return ERROR_CHECK_ADD_PROCESS; // check current process is coordinator.
  }

  running_ptr->last_syscall = ADD_PROCESS_SYSCALL;

  pID_t new_pID = AddProcess(priority, parent_pID, first_instruction, user_stack_addr,
                             kernel_stack_addr, excepter_pID, pager_pID);
  UnlockBus();

  return new_pID;
}
// @C-
// @A+
pID_t
AddProcess(uint32_t priority, pID_t parent_pID, uint32_t first_instruction,
           uint32_t user_stack_addr, uint32_t kernel_stack_addr,
           pID_t excepter_pID, pID_t pager_pID)
{
  if (num_processes >= *boot_max_processes)
  { return ERROR_PROCESS_TABLE_FULL; }

  int add_thread = 0;

  if ((priority & THREAD_FIELD) != 0)
  { add_thread = 1; }

  priority          &= 0xF; //mask all but priority
  process_table_t* pID_ptr  = first_process_ptr;
  process_table_t* parent_ptr;

  if (parent_pID == USE_CURRENT_PROCESS) {
    parent_pID = running_ptr->pID;
    parent_ptr = running_ptr;
  } else
  { parent_ptr = pIDtoPtr(parent_pID); }

  if (parent_ptr < 0)
  { return ERROR_ADD_PROCESS; }

  // search for empty entry in process table, then set values for new entry.
  // Assume this succeeds since we tested num_processes above.
  while (pID_ptr->status != EMPTY && pID_ptr < last_process_ptr)
  { pID_ptr++; }

  // reset version if last version was maximum
  if ((pID_ptr->pID & VERSION_MASK) == VERSION_MASK)
  { pID_ptr->pID &= PID_MASK; }

  pID_ptr->pID++;
  pID_ptr->status = PENDING;

  // Add Memory for the process/thread, and stack pages
  pID_ptr->size = PAGE_SIZE;

  if (add_thread == 0) {
    if (AddMemory(pID_ptr) < OK) { //this sets base page.
      pID_ptr->status = EMPTY;

      return ERROR_ADD_MEMORY;
    }
    pID_ptr->privilege |= ADDR_SPACE_OWNER;
  } else // use parent's address space.
  { pID_ptr->base_page = parent_ptr->base_page; }

  // The following values must be set before calling AddProcessMec()
  pID_ptr->first_instruction  = first_instruction;
  pID_ptr->user_stack_addr  = user_stack_addr;
  pID_ptr->kernel_stack_addr  = kernel_stack_addr;
  int ks_result       = AddPage(pID_ptr, kernel_stack_addr, KERNEL_RW);

  if ((ks_result != ERROR_PAGE_ALREADY_MAPPED) && (ks_result < OK))
  { return ERROR_ADD_PROCESS; }

  int us_result       = AddPage(pID_ptr, user_stack_addr, USER_RW);

  if ((us_result != ERROR_PAGE_ALREADY_MAPPED) && (us_result < 1)) {
    if (add_thread == 0)
    { RemoveMemory(pID_ptr); }
    else if (ks_result > 0)
    { UnmapPage(pID_ptr, kernel_stack_addr); }

    return ERROR_ADD_PROCESS;
  }

  // Add Process Mechanisms
  if (AddProcessMec(pID_ptr) < OK) { //this depends on AddMemory succeeding.
    pID_ptr->status = EMPTY;

    if (add_thread == 0) {
      RemoveMemory(pID_ptr);
    } else {
      if (ks_result > 0)
      { UnmapPage(pID_ptr, kernel_stack_addr); }

      UnmapPage(pID_ptr, user_stack_addr);
    }
    return ERROR_ADD_PROCESS_MEC;
  }

  // Fill in the rest of the Process Table values
  pID_ptr->reason = REASON_PENDING;
  pID_ptr->priority = priority; // this includes bit 4 to indicate whether it owns the AS.
  pID_ptr->current_priority = 0; // not used.
  pID_ptr->times_scheduled = 0;
  pID_ptr->time_blocked = 0;
  pID_ptr->blocked_ticks = 0;
  pID_ptr->time_started = (clock_seconds * TICK_FREQ) + clock_ticks;
  pID_ptr->time_running = 0;
  pID_ptr->page_maps = EMPTY_PAGE_MAP;
  pID_ptr->excepter_pID = excepter_pID;
  pID_ptr->pager_pID = pager_pID;
  pID_ptr->next = NULL;
  pID_ptr->prev = NULL;
  pID_ptr->own_msgQn = pID_ptr->own_msgQp = pID_ptr;
  pID_ptr->wait_msgQn = pID_ptr->wait_msgQp = NULL;
  //pID_ptr->privilege |= USER_PRIVILEGE; //this is 0, so not needed.

  // Create the family tree structure
  pID_ptr->parent_pID = parent_pID;
  pID_ptr->child = 0;
  pID_ptr->sibling = 0;
  pID_ptr->next_thread = 0;
  pID_ptr->thread_owner = pID_ptr; // this is changed for new threads.

  if (add_thread == 1) {
    pID_ptr->thread_owner = parent_ptr;

    if (parent_ptr->next_thread == 0) {
      parent_ptr->next_thread = pID_ptr;
    } else {
      parent_ptr = parent_ptr->next_thread;

      while (parent_ptr->next_thread != 0)
      { parent_ptr = parent_ptr->next_thread; }

      parent_ptr->next_thread = pID_ptr;
    }
  } else if (parent_ptr > 0 && parent_ptr != kernel_ptr) {
    if (parent_ptr->child == 0) {
      parent_ptr->child = pID_ptr;
    } else {
      parent_ptr = parent_ptr->child;

      while (parent_ptr->sibling != 0)
      { parent_ptr = parent_ptr->sibling; }

      parent_ptr->sibling = pID_ptr;
    }
  }
  num_processes++;

  return pID_ptr->pID;
}
// @A-

int32_t
RemoveProcessCall(int32_t pID)
{
  process_table_t*  pID_ptr = 0;
  int32_t       result;

  LockBus();

  if (pID == USE_CURRENT_PROCESS)
  { pID_ptr = running_ptr; }

  else if (!(pID_ptr = pIDtoPtr(pID))) {
    UnlockBus();
    return ERROR_PID_PTR;
  }
  if (pID == USE_CURRENT_PROCESS || CheckCurrent(pID_ptr) > 0) {
    running_ptr->last_syscall = REMOVE_PROCESS_SYSCALL;
    result            = RemoveProcess(pID_ptr);

    UnlockBus();

    return result;
  } else {
    UnlockBus();
    return 0;
  }
}

// @A+
int32_t
RemoveProcess(process_table_t* pID_ptr)
{
  int AS_owner = 0;

  if ((pID_ptr->privilege & ADDR_SPACE_OWNER) == ADDR_SPACE_OWNER)
  { AS_owner = 1; }

  if (pID_ptr->next_thread != NULL && AS_owner == 1) {
    return ERROR_REMOVE_PROCESS_RESOURCES_CHECK;
  } else if (AS_owner == 0) {
    process_table_t* thread = pID_ptr->thread_owner;

    while (thread->next_thread != pID_ptr)
    { thread = thread->next_thread; }

    thread->next_thread = pID_ptr->next_thread;
  }

  pID_ptr->thread_owner = 0;
  pID_ptr->next_thread  = 0;
  int schedule_required = 0;

  if (pID_ptr == running_ptr)
  { schedule_required++; }

  num_processes--;

  if (pID_ptr->status == BLOCKED)
  { RemoveQueue(&blockedQ, pID_ptr); }
  else if (pID_ptr->priority < FAST_PRIORITY)
  { RemoveQueue(&readyQ, pID_ptr); }
  else
  { RemoveQueue(&ready_fastQ, pID_ptr); }

  pID_ptr->status = EMPTY;
  pID_ptr->reason = REASON_REMOVED;

  RemoveProcessMessages(pID_ptr);
  RemoveProcessResources(pID_ptr);
  RemoveProcessMec(pID_ptr);

  if (AS_owner == 1) {
    RemoveMemory(pID_ptr);
    RemoveChild(pID_ptr);
  }
  pID_ptr->parent_pID = 0;

  //commented lines are useful values to leave in process table.
  //  pID_ptr->pID = 0;
  //  pID_ptr->last_syscall = 0;
  pID_ptr->priority = 0;
  pID_ptr->current_priority = 0;
  pID_ptr->times_scheduled = 0;
  pID_ptr->privilege = 0;
  pID_ptr->time_started = 0;
  pID_ptr->time_running = 0;
  pID_ptr->excepter_pID = 0;
  pID_ptr->pager_pID = 0;
  pID_ptr->next = NULL;
  pID_ptr->prev = NULL;
  pID_ptr->first_instruction = 0;
  pID_ptr->user_stack_addr = 0;
  pID_ptr->kernel_stack_addr = 0;
  pID_ptr->base_page = 0;
  pID_ptr->size = 0;
  pID_ptr->msg_range_length = 0;
  pID_ptr->timer = 0;
  pID_ptr->prev_msg_target_pID = 0;

  if (schedule_required) {
    Schedule();
  }
  return OK;
}
// @A-
// @C+
///////////////////////////////////////////////////////////////////////////////////////////////////
///// Check, Get, Set Interfaces //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int
Check(void)
{
  // Check capabilities for current process. Only allow if coordinator, or no coordinator is set yet.
  if (coord_set == 0) {
    return OK; // coordinator not set yet, standard condition at kernel init.
  } else if ((running_ptr->privilege & COORD_PRIVILEGE) == COORD_PRIVILEGE) {
    return OK;
  } else {
    num_check_failures++;
    SetProcess(running_ptr, P_REASON, ERROR_CHECK_FAILED);
    return ERROR_CHECK_FAILED;
  }
}

int
CheckCurrent(process_table_t* pID_ptr)
{
  /* Check current process matches the request, is a coordinator, or the parent_pID, or
   * excepter/pager.*/
  if (pID_ptr == running_ptr
      || pID_ptr->excepter_pID  == running_ptr->pID
      || pID_ptr->pager_pID   == running_ptr->pID
      || pID_ptr->parent_pID    == running_ptr->pID
      || Check()
     ) {
    return OK;
  } else {
    num_check_failures++;
    return ERROR_CHECK_FAILED;
  }
}
// @C-
// @G+ Get/Set Functions
uint32_t
GetProcessCall(int32_t pID, uint32_t* properties)
{
  process_table_t*  pID_ptr;
  uint32_t      result;

  LockBus();

  if (pID == 0) {
    running_ptr->last_syscall = GET_PROCESS_SYSCALL;
    result            = GetProcess(0, properties);

    UnlockBus();
    return result;
  }

  if (pID == USE_CURRENT_PROCESS) {
    pID_ptr = running_ptr;
  } else if (!(pID_ptr = pIDtoPtr(pID))) {
    UnlockBus();
    return ERROR_PID_PTR;
  }

  if (CheckCurrent(pID_ptr)) {
    running_ptr->last_syscall = GET_PROCESS_SYSCALL;
    result            = GetProcess(pID_ptr, properties);
    UnlockBus();
    return result;
  } else {
    UnlockBus();
    return ERROR_GET_PROCESS;
  }
}

uint32_t
GetProcess(process_table_t* pID_ptr, uint32_t* properties)
{
  if (pID_ptr == 0) { // cases where pID_ptr is irrelevant
    switch (properties[0]) {
      case K_NUM_PROCS : {
        properties[0] = num_processes;
        break;
      }
      case K_COORD : {
        pID_ptr = first_process_ptr;

        while ((pID_ptr->privilege & COORD_PRIVILEGE) != COORD_PRIVILEGE
               && pID_ptr <= last_process_ptr) {
          pID_ptr++;
        }

        if (pID_ptr <= last_process_ptr) {
          properties[0] = (uint32_t)pID_ptr->pID;
          break;
        } else {
          return ERROR_NO_COORDINATOR;
        }
      }
      case K_TIME : {
        properties[0] = (clock_ticks + clock_seconds * TICK_FREQ);
        break;
      }
      case K_MAX_PROCESSES : {
        properties[0] = *boot_max_processes;
        break;
      }
      default: {
        return ERROR_GET_PROCESS_PROPERTY;
      }
    }
  } else {
    switch (properties[0]) {
      case P_PRIVILEGE: {
        properties[0] = (uint32_t)running_ptr->privilege;
        break;
      }
      case P_PID: {
        properties[0] = (uint32_t)running_ptr->pID;
        break;
      }
      case P_BASEPAGE: {
        properties[0] = (uint32_t)pID_ptr->base_page;
        break;
      }
      case P_STATUS: {
        properties[0] = (uint32_t)pID_ptr->status;
        break;
      }
      case P_REASON: {
        properties[0] = (uint32_t)pID_ptr->reason;
        break;
      }
      case P_SIZE: {
        properties[0] = (uint32_t)pID_ptr->size;
        break;
      }
      case P_RUNNING_TIME: {
        properties[0] = (uint32_t)pID_ptr->time_running;
        break;
      }
      case P_START_TIME: {
        properties[0] = (uint32_t)pID_ptr->time_started;
        break;
      }
      case P_CURR_PRI: {
        properties[0] = (uint32_t)pID_ptr->current_priority;
        break;
      }
      case P_PRIORITY: {
        properties[0] = (uint32_t)pID_ptr->priority;
        break;
      }
      case P_PARENT: {
        properties[0] = (uint32_t)pID_ptr->parent_pID;
        break;
      }
      case P_USTACK: {
        properties[0] = pID_ptr->user_stack_addr;
        break;
      }
      case P_KSTACK: {
        properties[0] = pID_ptr->kernel_stack_addr;
        break;
      }
      case P_MEM_RIGHTS: {
        properties[0] = GetMemoryMec(pID_ptr, 0xFFFFFFFFu, properties[1]);
        // properties[1] is the virtual address, the properties[0] result is the page
        // properties, eg RWX, dirty, accessed.
        break;
      }
      default: {
        return ERROR_GET_PROCESS_PROPERTY;
      }
    }
  }
  return OK;
}

int
SetProcessCall(pID_t pID, int property, uint32_t value)
{
  process_table_t* pID_ptr;
  int result;

  LockBus();

  if (pID == USE_CURRENT_PROCESS) {
    pID_ptr = running_ptr;
  } else if (!(pID_ptr = pIDtoPtr(pID))) {
    UnlockBus();
    return ERROR_PID_PTR;
  }

  if (pID == USE_CURRENT_PROCESS || CheckCurrent(pID_ptr)) {
    running_ptr->last_syscall = SET_PROCESS_SYSCALL;
    result            = SetProcess(pID_ptr, property, value);
    UnlockBus();

    return result;
  } else {
    UnlockBus();
    return ERROR_SET_PROCESS;
  }
}

int
SetProcess(process_table_t* pID_ptr, int property, uint32_t value)
{
  switch (property) {
    case P_REASON: {
      pID_ptr->reason = value;
      break;
    }
    case P_PRIORITY: {
      Check();
      // Need to move the process from one Ready queue to another if it's priority has
      // changed enough.
      uint8_t curr_queue  = NORMAL_PRIORITY;
      uint8_t new_queue = NORMAL_PRIORITY;

      if (pID_ptr->priority >= FAST_PRIORITY)
      { curr_queue = FAST_PRIORITY; }

      if (value >= FAST_PRIORITY)
      { new_queue = FAST_PRIORITY; }

      if (curr_queue != new_queue && pID_ptr->status != BLOCKED) {
        if (curr_queue == FAST_PRIORITY) {
          RemoveQueue(&ready_fastQ, pID_ptr);
        } else {
          RemoveQueue(&readyQ, pID_ptr);
        }

        if (new_queue == FAST_PRIORITY) {
          AddQueue(&ready_fastQ, pID_ptr);
        } else {
          AddQueue(&readyQ, pID_ptr);
        }
      }

      pID_ptr->priority = value;
      break;
    }
    case P_PRIVILEGE: {
      Check();

      pID_ptr->privilege |= value;

      if ((value & COORD_PRIVILEGE) == COORD_PRIVILEGE)
      { coord_set = 1; } //indicates coordinator has been set.

      break;
    }
    case P_PARENT: {
      pID_ptr->parent_pID = value;
      break;
    }
    case P_BLOCKED: {
      if (pID_ptr->status != READY && pID_ptr->status != RUNNING) {
        return ERROR_SET_BLOCKED;
      } else {
        if (pID_ptr->priority < FAST_PRIORITY)
        { RemoveQueue(&readyQ, pID_ptr); }
        else
        { RemoveQueue(&ready_fastQ, pID_ptr); }

        AddQueue(&blockedQ, pID_ptr);

        pID_ptr->status     = BLOCKED;
        pID_ptr->reason     = value;
        pID_ptr->blocked_ticks  = (clock_seconds * TICK_FREQ) + clock_ticks;
      }
      break;
    }
    case P_READY: {
      if (pID_ptr->status == BLOCKED) {
        RemoveQueue(&blockedQ, pID_ptr);

        pID_ptr->time_blocked = (clock_seconds * TICK_FREQ) + clock_ticks
                                - pID_ptr->blocked_ticks;
      } else if (pID_ptr->status == PENDING) {
        /* Don't add to any ready queue. Don't need to remove from blockedQ queue as
         * process is queue-less.*/
        if (pID_ptr->priority == 0)
        { break; }
      } else if (pID_ptr->status == READY) {
        pID_ptr->reason = value;
        return ERROR_SET_PROCESS_PROPERTY;
      }

      if (pID_ptr->priority < FAST_PRIORITY)
      { AddQueue(&readyQ, pID_ptr); }
      else
      { AddQueue(&ready_fastQ, pID_ptr); }

      pID_ptr->reason = value;
      pID_ptr->status = READY;

      if (pID_ptr->timer > 0) {
        if (num_sleepers > 0)
        { num_sleepers--; }

        pID_ptr->timer = 0;
      }
      break;
    } // end case P_READY.
    default: {
      return ERROR_SET_PROCESS_PROPERTY;
    }
  }
  return OK;
}
// @G- end Get/Set Functions
// @C+
process_table_t*
pIDtoPtr(pID_t pID)
{
  if (pID == KERNEL_PID)
  { return kernel_ptr; }
  else if (pID == ANY)
  { return any_ptr; }

  // ____________________________________
  // E | Trans | Match | Index | Version|
  // 1 |   5   |   4   |   10  |    12  |
  // ------------------------------------
  // remove transparent user-defined bits
  // if 5 bits, mask is 1000.0011.1111.1111... or 0x83FFFFFF;

  pID &= pID_transparent_mask;

  if (pID < MIN_PID || pID > max_pID)
  { return 0; }

  int32_t index       = pID >> VERSION_SHIFT;
  process_table_t* pID_ptr  = &processT[index];

  if (pID_ptr->pID != pID)
  { return 0; } //check that version matches.
  else
  { return pID_ptr; }
}
// @C-
// @G+ Get/Set Process queue functions.

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Process Management Local Functions //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int
AddQueue(process_table_t * *queue, process_table_t* pID_ptr)
{
  if (*queue != 0) { //if queue is not empty:
    (*queue)->prev->next  = pID_ptr;
    pID_ptr->next     = *queue;
    pID_ptr->prev     = (*queue)->prev;
    (*queue)->prev      = pID_ptr;
  } else { //if queue is empty:
    *queue      = pID_ptr;
    pID_ptr->prev = *queue;
    pID_ptr->next = *queue;
  }
  return OK;
}

int
RemoveQueue(process_table_t * *queue, process_table_t* pID_ptr)
{
  if (pID_ptr->next == pID_ptr) {
    // If queue is going to become empty, check that pID_ptr points to itself.
    *queue = NULL;
  } else {
    // If the queue is not about to become empty:
    pID_ptr->prev->next = pID_ptr->next;
    pID_ptr->next->prev = pID_ptr->prev;

    if (*queue == pID_ptr) {
      (*queue) = pID_ptr->prev;
      //When the target to be removed is the entry-point to the queue for the
      //main queue pointer queue, then the queue pointer needs to point to another
      //member of the queue. This is set to the last member assuming that if
      //Schedule is going to be called, it will select the next one after the new
      //queue pointer.
    }
  }
  return OK;
}

int
RemoveChild(process_table_t* pID_ptr)
{
  // Remove target pID_ptr from its parent's list.
  process_table_t* parent_ptr = pIDtoPtr(pID_ptr->parent_pID);

  if (parent_ptr == NULL)
  { return -1; } // this shouldn't happen.

  if (parent_ptr->child == NULL)
  { return -1; } // this shouldn't happen.

  if (parent_ptr->child == pID_ptr) {
    // pID_ptr->sibling may be next in list, or null.
    parent_ptr->child = pID_ptr->sibling;
  } else {
    process_table_t* sibling_ptr = parent_ptr->child;

    while (sibling_ptr->sibling != pID_ptr) {
      sibling_ptr = sibling_ptr->sibling;

      if (sibling_ptr == NULL)
      { return -1; } // this shouldn't happen.
    }

    sibling_ptr->sibling = pID_ptr->sibling;
  }
  pID_ptr->child = pID_ptr->sibling = NULL;

  return OK;
}
// @G- end Get/Set Process queue functions.

/* END OF FILE */
