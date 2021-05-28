///////////////////////////////////////////////////////////////////////////////////////////////////
/* Messaging Management
  messages.c

  This implements blocking send and receive.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @S web_messages_send Message Send/Receive
// @E web_messages_searching Searching for Messages
// @T web_messages_storing Storing Messages
// @I web_messages_timeouts Message Timeouts
// @S+
// @E+
// @T+
// @I+

#include "global.h"
#include "mec.h"
// @E-

///// Message Management Interfaces ///////////////////////////////////////////////////////////////
// @T-
// @I-
pID_t Send(pID_t receiver_pID, uint32_t* sender_msgB, uint32_t length, uint32_t timeout);
pID_t Receive(pID_t sender_pID, uint32_t* receiver_msgB, uint32_t length, uint32_t timeout);
// @T+
int   RemoveProcessMessages(process_table_t* pID_ptr);
// @T-
// @I+
int   UpdateTimers(void);
// @I-
// @E+
///// Message Management Local Functions //////////////////////////////////////////////////////////

process_table_t* SearchMessages(pID_t target_pID, int direction, int range, int length);
process_table_t* SearchMsgQueue(process_table_t* requestorQ_ptr,
                                process_table_t* target_ptr, int direction,
                                int target_range, int requestor_range,
                                int requestor_length);
// @E-
// @T+
int AddMessage(pID_t target_pID);
int RemoveMessage(process_table_t* target_ptr);
// @T-
int Deadlock(void);
// @S-

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern process_table_t* pIDtoPtr(pID_t pID);

extern int Schedule(void);
extern int SetProcess(process_table_t* pID_ptr, int property, uint32_t value);

///// Local Data //////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Message Management Interfaces ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// @S+
pID_t
Send(pID_t receiver_pID, uint32_t* sender_msgB, uint32_t length, uint32_t timeout)
{
  // @S-
  /* #if KERNEL_DEBUG
  if (message_log_index>=MSG_LOG_MAX) message_log_index=0;
  message_log[msg_log_index++] = msg_log_index/4;
  message_log[msg_log_index++] = current_pID;
  message_log[msg_log_index++] = REASON_SEND;
  message_log[msg_log_index++] = receiver_pID;
  #endif */
  // @S+

  LockBus();

  /* Configure arguments (depending upon the paths taken, not all of these need to be done, but
   * for debugging it is neater to have them together.*/
  running_ptr->last_syscall     = SEND_SYSCALL;
  running_ptr->prev_msg_target_pID  = receiver_pID;
  running_ptr->msg_range_length   = length;
  int range             = length & MSG_RANGE_MASK;

#if CPU_PORTS
  if (receiver_pID >= PORT_PID_FIRST && receiver_pID <= PORT_PID_LAST) {
    int result = WritePort(receiver_pID, sender_msgB, length);
    UnlockBus();

    return result;
  } else
#endif

    if (receiver_pID < MIN_PID && receiver_pID > max_pID) {
      // if target is outside process limits return error.

      SetProcess(running_ptr, P_REASON, ERROR_SEND_FAILED);
      UnlockBus();
      return ERROR_SEND_FAILED;
    }
  // else ... carry on.

  length = length & MSG_LENGTH_MASK;

  if (length > MSG_MAX_LENGTH)
  { length = MSG_MAX_LENGTH; }

  int i;
  process_table_t* receiver_ptr;

  //Check();
  receiver_ptr = SearchMessages(receiver_pID, REASON_RECEIVE, range, length);

  if (receiver_ptr == (process_table_t*)ERROR_DEADLOCK) {
    return ERROR_DEADLOCK;
  } else if (receiver_ptr == (process_table_t*)ERROR_PID_PTR) {
    return ERROR_SEND_FAILED_NO_TARGET;
  }

  if (receiver_ptr != 0) { // true if message found
    receiver_pID = receiver_ptr->pID; // in case receiver target was ANY.

    // copy Sender's msg to Receiver's buffer
    for (i = 0; i < length; i++)
    { receiver_ptr->msg[i] = *sender_msgB++; }

    SetProcess(receiver_ptr, P_READY, REASON_RECEIVE_DONE);
    SetProcess(running_ptr, P_REASON, REASON_SEND_DONE);
    RemoveMessage(receiver_ptr);

    //THREADS: receiver_ptr->prev_msg_target_pID = running_ptr->pID;
    // This is to update sender in case receiver specified ANY as its target.
  } else { //message not found
    if (!AddMessage(receiver_pID)) {
      SetProcess(running_ptr, P_REASON, ERROR_SEND_FAILED_NO_TARGET);
      UnlockBus();
      return ERROR_SEND_FAILED_NO_TARGET;
    }
    // copy Sender's message to Sender's buffer
    for (i = 0; i < length; i++)
    { running_ptr->msg[i] = *sender_msgB++; }

    running_ptr->timer = timeout;

    if (timeout > 0)
    { num_sleepers++; }

    SetProcess(running_ptr, P_BLOCKED, REASON_SEND);
    Schedule();

    // check reason field to ensure message was successful.
    if (running_ptr->reason != REASON_SEND_DONE
        && running_ptr->reason != REASON_EXCEPTION_DONE) {
      UnlockBus();
      return ERROR_SEND_FAILED;
    }
    //THREADS: receiver_pID = running_ptr->prev_msg_target_pID;
    // Update receiver_pID in case ANY has been replaced by actual pID.
  }
  receiver_pID = running_ptr->prev_msg_target_pID;
  UnlockBus();

  return receiver_pID;
}

pID_t
Receive(pID_t sender_pID, uint32_t* receiver_msgB, uint32_t length, uint32_t timeout)
{
  // @S-
  /* #if KERNEL_DEBUG
  if (message_log_index>=MSG_LOG_MAX) message_log_index=0;
  message_log[msg_log_index++] = msg_log_index/4;
  message_log[msg_log_index++] = current_pID;
  message_log[msg_log_index++] = REASON_RECEIVE;
  message_log[msg_log_index++] = sender_pID;
  #endif */
  // @S+

  LockBus();
  /* Configure arguments (depending upon the paths taken, not all of these need to be
   * done, but for debugging it is neater to have them together.*/

  running_ptr->last_syscall     = RECEIVE_SYSCALL;
  running_ptr->prev_msg_target_pID  = sender_pID;
  running_ptr->msg_range_length   = length;
  int range             = length & MSG_RANGE_MASK;

  // Read from I/O Port
  if (sender_pID >= PORT_PID_FIRST && sender_pID <= PORT_PID_LAST) {
    int result = ReadPort(sender_pID, receiver_msgB, length);
    // the data is placed in msg[0].

    // receiver_msgB[0] = running_ptr->msg[0];
    UnlockBus();

    return result;
  } else if (sender_pID < MIN_PID && sender_pID > max_pID) {
    // if target is outside process & interrupt limits.

    SetProcess(running_ptr, P_REASON, ERROR_SEND_FAILED);
    UnlockBus();
    return ERROR_SEND_FAILED;
  }

  length = length & MSG_LENGTH_MASK;
  if (length > MSG_MAX_LENGTH)
  { length = MSG_MAX_LENGTH; }

  int i, interrupt_flag = 0, interrupt_wakeup = 0;
  process_table_t* sender_ptr = 0;

  //Check();
  if (sender_pID == CLOCK_INTERRUPT) {
    /* by doing nothing here, the test for sender_ptr and wakeups will fail
    so the Receive call will jump to section to add the message. */
  } else if (sender_pID >= HWINT_FIRST && sender_pID <= HWINT_LAST) {
    // if Hardware Interrupt

    if (interruptT[sender_pID - INTERRUPT_OFFSET].pID != running_ptr->pID) {
      SetProcess(running_ptr, P_REASON, ERROR_NOT_INTERRUPT_HANDLER);
      UnlockBus();

      return ERROR_NOT_INTERRUPT_HANDLER;
    } else {
      interrupt_flag    = 1;
      length        = 0;
      interrupt_wakeup  = interruptT[sender_pID - INTERRUPT_OFFSET].wakeups;
    }
  } else { // search for matching waiting message
    sender_ptr = SearchMessages(sender_pID, REASON_SEND, range, length);

    if (sender_ptr == (process_table_t*)ERROR_DEADLOCK)
    { return ERROR_DEADLOCK; }

    else if (sender_ptr == (process_table_t*)ERROR_PID_PTR)
    { return ERROR_RECEIVE_FAILED_NO_TARGET; }
  }
  if (sender_ptr != 0 || interrupt_wakeup > 0) { // true if message found
    if (interrupt_flag) {
      interruptT[sender_pID - INTERRUPT_OFFSET].wakeups--;
      SetProcess(running_ptr, P_REASON, REASON_INTERRUPT_DONE);
    } else {
      // copy Sender's buffer to Receiver's msg

      //check for matching message lengths
      //if ((sender_ptr->msg_range_length & MSG_LENGTH_MASK) != length)
      //return sender_ptr->msg_range_length & MSG_LENGTH_MASK;

      for (i = 0; i < length; i++)
      { *receiver_msgB++ = sender_ptr->msg[i]; }

      RemoveMessage(sender_ptr);
      sender_pID = sender_ptr->pID; // in case sender target was ANY or interrupt.
      SetProcess(sender_ptr, P_READY, REASON_SEND_DONE);
      SetProcess(running_ptr, P_REASON, REASON_RECEIVE_DONE);

      //THREADS: sender_ptr->prev_msg_target_pID = running_ptr->pID;
      /* This is to update receiver in case sender specified ANY as its target
       * (ie in matching Send call).*/
    }
  } else { //message not found
    if (!AddMessage(sender_pID)) {
      SetProcess(running_ptr, P_REASON, ERROR_RECEIVE_FAILED_NO_TARGET);
      UnlockBus();
      return ERROR_RECEIVE_FAILED_NO_TARGET;
    }

    running_ptr->timer = timeout;

    if (timeout > 0)
    { num_sleepers++; }

    if (interrupt_flag && sender_pID != CLOCK_INTERRUPT)
    { EnableInterrupt(sender_pID); }

    SetProcess(running_ptr, P_BLOCKED, REASON_RECEIVE);
    Schedule();

    // check reason field to ensure message was successful.
    if (running_ptr->reason != REASON_RECEIVE_DONE
        && running_ptr->reason != REASON_INTERRUPT_DONE) {
      UnlockBus();
      return ERROR_RECEIVE_FAILED;
    }

    // copy Receiver's buffer to Receiver's message
    if (!interrupt_flag)
      for (i = 0; i < length; i++)
      { *receiver_msgB++ = running_ptr->msg[i]; }

    //THREADS: sender_pID = running_ptr->prev_msg_target_pID;
    //update sender in case ANY has been replaced by actual pID
  }
  sender_pID = running_ptr->prev_msg_target_pID;
  UnlockBus();

  return sender_pID;
}
// @S-
// @T+
int
RemoveProcessMessages(process_table_t* pID_ptr)
{
  process_table_t* temp_ptr;
  process_table_t* temp_ptr2;

  // wake up each process waiting on pID_ptr, and clear waiting queue
  temp_ptr = pID_ptr->own_msgQn;

  while (temp_ptr != pID_ptr) {
    SetProcess(temp_ptr, P_READY, REASON_MESSAGE_TARGET_REMOVED);
    temp_ptr2 = temp_ptr;
    temp_ptr = temp_ptr->wait_msgQn;
    temp_ptr2->wait_msgQn = temp_ptr2->wait_msgQp = 0;
  }

  pID_ptr->own_msgQn = pID_ptr->own_msgQp = pID_ptr;

  // clear pID_ptr's pending entry. This will use pID_ptr's pending queue
  // and skip over it on that queue.
  RemoveMessage(pID_ptr);

  for (int i = 0; i < MSG_MAX_LENGTH; i++)
  { pID_ptr->msg[i] = 0; }

  return OK;
}
// @T-

// @I+
int
UpdateTimers(void)
{
  process_table_t* pID_ptr = first_process_ptr;

  while (pID_ptr < last_process_ptr) {
    if (pID_ptr->timer > 0) {
      pID_ptr->timer--;

      if (pID_ptr->timer <= 0 && pID_ptr->status == BLOCKED) {
        num_sleepers--;

        switch (pID_ptr->reason) {
          case REASON_SEND: {
            SetProcess(pID_ptr, P_READY, REASON_SEND_TIMEOUT);
            RemoveMessage(pID_ptr);
            break;
          }
          case REASON_RECEIVE: {
            //need to check whether pID_ptr is waiting on interrupt, and
            //if so disable IRQ.
            if (pID_ptr->prev_msg_target_pID >= HWINT_FIRST &&
                pID_ptr->prev_msg_target_pID <= HWINT_LAST &&
                pID_ptr->prev_msg_target_pID != CLOCK_INTERRUPT) {
              DisableInterrupt(pID_ptr->prev_msg_target_pID);
            }

            SetProcess(pID_ptr, P_READY, REASON_RECEIVER_TIMEOUT);
            RemoveMessage(pID_ptr);
            break;
          }
        }
      }
    }
    pID_ptr++;
  }
  return OK;
}
// @I-

// @E+
///////////////////////////////////////////////////////////////////////////////////////////////////
///// Message Management Local Functions //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

process_table_t*
SearchMessages(pID_t target_pID, int direction, int range, int length)
{
  process_table_t* target_ptr = pIDtoPtr(target_pID);

  if (target_ptr == 0)
  { return (process_table_t*)ERROR_PID_PTR; }

  process_table_t* result_ptr = NULL, *requestorQ_ptr = NULL, *thread_owner = NULL;

  // New Thread Code, note ranges are not used just yet.
  thread_owner = running_ptr->thread_owner;

  // 1. Check requestor's own_msgQn queue for target.
  result_ptr = SearchMsgQueue(running_ptr, target_ptr, direction, range, 0, length);

  if ((range & RTHREAD) == RTHREAD) {
    if (result_ptr == 0) { // check thread owner (if it isn't current running thread)
      if (thread_owner != running_ptr && thread_owner != 0)
        result_ptr = SearchMsgQueue(thread_owner, target_ptr, direction,
                                    range, TTHREAD, length);
    }
    requestorQ_ptr = thread_owner->next_thread;

    // check other threads belonging to thread owner.
    while (requestorQ_ptr != 0 && result_ptr == 0) {
      if (requestorQ_ptr != running_ptr) // don't check it again.
        result_ptr = SearchMsgQueue(requestorQ_ptr, target_ptr, direction,
                                    range, TTHREAD, length);

      requestorQ_ptr = requestorQ_ptr->next_thread;
    }
  }

  // 4. Check ANY queue
  if (result_ptr == 0 && target_ptr != any_ptr)
  { result_ptr = SearchMsgQueue(any_ptr, target_ptr, direction, range, 0, length); }

  return result_ptr;
}

process_table_t*
SearchMsgQueue(process_table_t* requestorQ_ptr,
               process_table_t* target_ptr, int direction,
               int target_range, int requestor_range,
               int requestor_length)
{
  process_table_t* msgQ_ptr, *parent_ptr, *result_ptr;
  msgQ_ptr = requestorQ_ptr->own_msgQn;

  // New Thread Code, as above range is not checked just yet.
  while (msgQ_ptr != requestorQ_ptr) {
    if (msgQ_ptr->reason == direction &&
        ((msgQ_ptr->msg_range_length & requestor_range) == requestor_range) &&
        ((msgQ_ptr->msg_range_length & MSG_LENGTH_MASK) == requestor_length)) {
      int found = 1;
      if (msgQ_ptr == target_ptr) {
        running_ptr->msg_range_length |= TSELF_ANSWERED; //msgQ == target
      } else if (target_ptr == any_ptr) {
        running_ptr->msg_range_length |= TANY_ANSWERED; //target == any
      } else if (msgQ_ptr->thread_owner == target_ptr->thread_owner
                 && ((target_range & TTHREAD) != 0)) {
        running_ptr->msg_range_length |= TTHREAD_ANSWERED; // thread owners same.
      } else {
        found = 0;
      }

      if (found == 1) {
        running_ptr->msg_range_length |= TSELF_ANSWERED;
        running_ptr->prev_msg_target_pID = msgQ_ptr->pID;
        msgQ_ptr->prev_msg_target_pID = running_ptr->pID;

        if (requestorQ_ptr->thread_owner == running_ptr)
        { msgQ_ptr->msg_range_length |= TSELF_ANSWERED; }
        else
        { msgQ_ptr->msg_range_length |= TTHREAD_ANSWERED; }

        return msgQ_ptr;
      }
    }
    msgQ_ptr = msgQ_ptr->wait_msgQn; // iterate loop
  }
  return 0;
}
// @E-

// @T+
int
AddMessage(pID_t target_pID)
{
  process_table_t* target_ptr;
  if (target_pID >= HWINT_FIRST && target_pID <= HWINT_LAST) {
    if (interruptT[target_pID - INTERRUPT_OFFSET].own_msgQn
        == (process_table_t*)target_pID) { // if queue is empty
      interruptT[target_pID - INTERRUPT_OFFSET].own_msgQn
        = (process_table_t* )running_ptr;
    } else { // if queue is not empty
      interruptT[target_pID - INTERRUPT_OFFSET].own_msgQp->wait_msgQn = running_ptr;
    }
    running_ptr->wait_msgQp = interruptT[target_pID - INTERRUPT_OFFSET].own_msgQp;
    running_ptr->wait_msgQn = (process_table_t* )target_pID;
    interruptT[target_pID - INTERRUPT_OFFSET].own_msgQp = running_ptr;
  } else {
    if (target_pID != ANY) {
      if (!(target_ptr = pIDtoPtr(target_pID)))
      { return 0; }
    } else {
      target_ptr = any_ptr;
    }
    if (target_ptr->own_msgQn == target_ptr) { // if queue is empty
      target_ptr->own_msgQn = running_ptr;
    } else { // if queue is not empty
      target_ptr->own_msgQp->wait_msgQn = running_ptr;
    }
    running_ptr->wait_msgQp = target_ptr->own_msgQp;
    running_ptr->wait_msgQn = target_ptr;
    target_ptr->own_msgQp = running_ptr;
  }

  return OK;
}

int
RemoveMessage(process_table_t* target_ptr)
{
  int interrupt_flag  = 0;
  int interrupt_index = 0;

  //Check if target's next pointer is to an interrupt
  interrupt_index = (int)target_ptr->wait_msgQn;

  if (interrupt_index >= HWINT_FIRST && interrupt_index <= HWINT_LAST)
  { interrupt_flag = 1; }

  if (target_ptr->wait_msgQn == target_ptr->wait_msgQp) { // only one on list
    if (target_ptr->wait_msgQn == 0) { return 0; }
    if (interrupt_flag) {
      interruptT[interrupt_index - INTERRUPT_OFFSET].own_msgQp
        = interruptT[interrupt_index - INTERRUPT_OFFSET].own_msgQn
          = (process_table_t*)interrupt_index;
    } else {
      target_ptr->wait_msgQn->own_msgQp
        = target_ptr->wait_msgQn->own_msgQn
          = target_ptr->wait_msgQn;
    }
  } else { //if not only process waiting on target:
    // Update target's next pointer:
    if (interrupt_flag) {
      interruptT[interrupt_index - INTERRUPT_OFFSET].own_msgQp
        = target_ptr->wait_msgQp;
    } else if (target_ptr->wait_msgQn->wait_msgQp == target_ptr) {
      target_ptr->wait_msgQn->wait_msgQp = target_ptr->wait_msgQp;
    } else { //if (target_ptr->wait_msgQn->own_msgQp == target_ptr)
      target_ptr->wait_msgQn->own_msgQp = target_ptr->wait_msgQp;
    }

    // Update target's previous pointer:
    interrupt_index = (int)target_ptr->wait_msgQp;

    if (interrupt_index >= HWINT_FIRST && interrupt_index <= HWINT_LAST) {
      interruptT[interrupt_index - INTERRUPT_OFFSET].own_msgQn
        = target_ptr->wait_msgQn;
    } else if (target_ptr->wait_msgQp->wait_msgQn == target_ptr) {
      target_ptr->wait_msgQp->wait_msgQn = target_ptr->wait_msgQn;
    } else { //if (target_ptr->wait_msgQp->own_msgQn == target_ptr)
      target_ptr->wait_msgQp->own_msgQn = target_ptr->wait_msgQn;
    }
  }

  target_ptr->wait_msgQn = 0;
  target_ptr->wait_msgQp = 0;

  return OK;
}
// @T-

int
Deadlock(void)
{
  return ERROR_DEADLOCK;
}

/* END OF FILE */
