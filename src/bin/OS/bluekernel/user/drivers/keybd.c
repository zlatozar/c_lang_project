///////////////////////////////////////////////////////////////////////////////////////////////////
/* Keyboard Driver
  keybd.c

  A basic keyboard driver that registers itself to receive keyboard interrupts, and reads the key
  scancode from the keyboard port. It converts scancodes into ASCII and displays some of them on
  the screen, others are converted into commands to send to the server.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

// Documentation annotations:
// @A web_development_keybd Keyboard Driver Code
// @A+

#include "keybd.h"

#include "../../server/server.h"
#include "../lib/syscall.h"
#include "../lib/syscallext.h"
#include "../lib/stdlib.h"

#define KB_INT        HWINT1
#define KB_PORT       0x60
#define KB_PORT_ID      (2 + PORT_PID_FIRST)
// this is the resource number in the kernel that maps to KB_PORT.

/* AT keyboard. These defines are from Minix2 keyboard.c */
#define KB_COMMAND      0x64  /* I/O port for commands on AT */
#define KB_GATE_A20     0x02  /* bit in output port to enable A20 line */
#define KB_PULSE_OUTPUT   0xF0  /* base for commands to pulse output port */
#define KB_RESET      0x01  /* bit in output port to reset CPU */
#define KB_STATUS     0x64  /* I/O port for status on AT */
#define KB_ACK        0xFA  /* keyboard ack response */
#define KB_BUSY       0x02  /* status bit set when KEYBD port Ready */
#define LED_CODE      0xED  /* command to keyboard to set LEDs */
#define MAX_KB_ACK_RETRIES  0x1000  /* max #times to wait for kb ack */
#define MAX_KB_BUSY_RETRIES 0x1000  /* max #times to loop while kb busy */
#define KBIT        0x80  /* bit used to ack characters to keyboard */

/* Miscellaneous ports. */
#define PCR         0x65  /* Planar Control Register */
#define PORT_B        0x61  /* I/O port for 8255 port B (kbd, beeper...) */
#define TIMER0        0x40  /* I/O port for Timer channel 0 */
#define TIMER2        0x42  /* I/O port for Timer channel 2 */
#define TIMER_MODE      0x43  /* I/O port for Timer mode control */

int     DoRequest(void);
int     Open(void);
void    RegisterKeyboard();

void    CapsToggle(void);
uint8_t   Keyboard_ISR(void);

void    StartCollectString(int new_command, char const* message);
void    CollectString(uint8_t ch);

int     OpenFile(char* filename);
int     StartThread(pID_t pID);

uint32_t  request[SERVER_MSG_SIZE];
uint32_t  reply[SERVER_MSG_SIZE];
int32_t   request_pID;

int     shift, caps, ctrl, alt, numlock;
uint8_t   scancode;
uint32_t  kb_portID = KB_PORT_ID;
uint32_t  server_msg[SERVER_MSG_SIZE];

int     name_ch;
int     command;
int     collect_string;
uint8_t   file_name[NAME_LENGTH];

pID_t   video_pID;
pID_t   server_pID;
pID_t   this_pID;

int
kmain(void)
{
  Open();

  while (1) {
    request_pID = Receive(KB_INT, request, SERVER_MSG_SIZE, 0); // Sleep on keyboard INT.
    DoRequest();
  }
}

int
DoRequest(void)
{
  if (request_pID == KB_INT) {
    Keyboard_ISR();
  } else {
    switch (request[MSG_TYPE]) {
      case MSG_CLOSE : {
        break;
      }
      case MSG_GET : {
        break;
      }
      case MSG_SET : {
        break;
      }
      default : {
        break;
      }
    }
  }

  return 1;
}

int
Open(void)
{
  collect_string = 0;

  for (int i = 0; i < NAME_LENGTH; i++)
  { file_name[i] = 0; }

  this_pID  = GetCurrentPid();
  server_pID  = GetCoordinator();

  GetSystemInfo();

  name_ch   = 0;
  command   = 0;

  shift   = SHIFT_OFF;
  caps    = CAPS_OFF;
  ctrl    = CTRL_OFF;
  alt     = ALT_OFF;
  numlock   = NUMLOCK_OFF;

  RegisterKeyboard();

  kprintf("Keybd ");

  return 1;
}

void
RegisterKeyboard(void)
{
  request[0] = ADD_RESOURCE_SYSCALL;
  request[1] = USE_CURRENT_PROCESS;
  request[2] = INT_RESOURCE;
  request[3] = KB_INT;

  Send(server_pID, request, SERVER_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);

  // check reply value from system call, ie AddResource.
  request[0] = ADD_RESOURCE_SYSCALL;
  request[2] = IOPORT_RESOURCE;
  request[1] = USE_CURRENT_PROCESS;
  request[3] = KB_PORT_ID;
  request[4] = KB_PORT;

  Send(server_pID, request, SERVER_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);

  kb_portID = KB_PORT_ID;
}

void
CapsToggle(void)
{
  caps = (caps == CAPS_OFF) ? CAPS_ON : CAPS_OFF;
}

uint8_t
Keyboard_ISR(void)
{
  uint8_t ch        = 0;
  int   key_released  = 0;
  int   key_pressed   = 0;
  int   numpad_pressed  = 0;

  Read8(kb_portID, &scancode);  /* get the scan code for the key */

  if (scancode >= KEY_RELEASED) {
    key_released  = 1;
    key_pressed   = 0;
    scancode    -= KEY_RELEASED;
  } else {
    key_released  = 0;
    key_pressed   = 1;
  }

  if (scancode == SC_LSHIFT || scancode == SC_RSHIFT) {
    // Regardless of whether a shift key was pressed or released, toggle the caps status.
    CapsToggle();
    shift = SHIFT_ON * key_pressed;
  } else if (scancode == SC_ALT) {
    alt = ALT_ON * key_pressed;
  } else if (scancode == SC_CTRL) {
    ctrl = CTRL_ON * key_pressed;
  } else if (key_pressed) {
    unsigned short key = keymap[(scancode * MAP_COLS)];

    if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
      key = keymap[(scancode * MAP_COLS) + caps];
    } else if (shift) {
      key = keymap[(scancode * MAP_COLS) + shift];
    }

    ch = (uint8_t)key;

    // Test whether a number pad key has been hit.
    if ((key & NUM) == NUM) {
      numpad_pressed = 1;

      // If the numlock is on, then remove the NUM field to reveal the number character.
      if (numlock)
      { key = key & (~NUM); }
    }

    if (ctrl) {
      key = Ctrl(key);
    }

    if (alt) {
      key = Alt(key);
    }

    if (collect_string == 1) {
      CollectString(ch);
    }

    switch (key) {
      case CAPS_LOCK : {
        CapsToggle();
        break;
      }
      case NUM_LOCK : {
        numlock = (numlock == NUMLOCK_ON ? NUMLOCK_OFF : NUMLOCK_ON);
        break;
      }
      case Ctrl(HOME) : {
        kprintf("Home");
        break;
      }
      case PGUP : {
        kprintf("Up");
        break;
      }
      case Ctrl('o'): {
        StartCollectString(MSG_OPEN_NAME, "Open:");
        break;
      }
      case Ctrl('P'): {
        StartCollectString(MSG_PAUSE_PID, "Pause:");
        break;
      }
      case Ctrl('R'): {
        StartCollectString(MSG_RUN_PID, "Run:");
        break;
      }
      case Ctrl('q'): {
        StartCollectString(MSG_CLOSE_PID, "Close:");
        break;
      }
      case Ctrl('t'): {
        StartCollectString(MSG_NEW_THREAD, "Thread:");
        break;
      }
      case Ctrl('z'): {
        kprintf("open 1+2.");
        OpenFile("user1.bin");
        OpenFile("user2.bin");
        break;
      }
      case Ctrl('x'): {
        kprintf("open T1/2.");
        OpenFile("threads1.bin");
        OpenFile("threads2.bin");
        break;
      }
      case Ctrl('X'): {
        kprintf("start threads.");
        StartThread(0x6001);
        StartThread(0x7001);
        break;
      }
      default: {
        /* We only want to display the character pressed when:
         *  - it isn't the ENTER key.
         *  - a modifier key (ctrl, alt) isn't held down.
         *  - if a number pad key is hit, then the numlock must be on.
         *  - it isn't an extended key (eg F1).
         */
        if ((ch != ENTER
             && !ctrl
             && !alt
             && !numpad_pressed
             && ((key & EXT) != EXT))
            || ((numpad_pressed == 1)
                && (numlock == NUMLOCK_ON))) {
          printc(ch);
        }

        break;
      }
    }
  }

  return ch;
}

void
StartCollectString(int new_command, char const* message)
{
  collect_string  = 1;
  name_ch     = 0;
  command     = new_command;
  kprintf(message);
}

void
CollectString(uint8_t ch)
{
  file_name[name_ch++] = ch;

  if (ch == ENTER || name_ch == NAME_LENGTH) { // end of name or number.
    file_name[name_ch - 1]  = 0;
    collect_string      = 0;
    name_ch         = 0;

    // if conversion to int is 0, assume we have file name.
    int number = atoi(file_name);

    if (number == 0) {
      if (command == MSG_OPEN_NAME) {
        OpenFile(file_name);
      }
    } else { // assume we have process number.
      if (command == MSG_PAUSE_PID) {
        server_msg[MSG_TYPE]    = MSG_SET;
        server_msg[MSG_USER_TYPE] = SET_PROCESS_SYSCALL;
        server_msg[2]       = number; // pID
        server_msg[3]       = P_BLOCKED;
        server_msg[4]       = REASON_BLOCKED;
      } else if (command == MSG_RUN_PID) {
        server_msg[MSG_TYPE]    = MSG_SET;
        server_msg[MSG_USER_TYPE] = SET_PROCESS_SYSCALL;
        server_msg[2]       = number; // pID
        server_msg[3]       = P_READY;
        server_msg[4]       = USER_REASON_1;
      } else if (command == MSG_CLOSE_PID) {
        server_msg[MSG_TYPE]    = MSG_CLOSE_PID;
        server_msg[MSG_USER_TYPE] = number;
      } else if (command == MSG_NEW_THREAD) {
        server_msg[MSG_TYPE]    = MSG_NEW_THREAD;
        server_msg[MSG_USER_TYPE] = number;
      }

      Send(server_pID, server_msg, SERVER_MSG_SIZE, 0);
      Receive(server_pID, server_msg, SERVER_MSG_SIZE, 0);

      if (server_msg[0] == OK) {
        kprintf(" OK");
      } else {
        kprintf(" Err: ");
        printx(server_msg[0], 4);
      }
    }
  }
}

int
OpenFile(char* filename)
{
  server_msg[MSG_TYPE]    = MSG_OPEN_NAME; //MSG_OPEN;
  server_msg[MSG_USER_TYPE] = this_pID; // parent process.
  server_msg[2]       = NORMAL_PRIORITY;

  // copy the file name to the message bufffer.
  uint8_t* msg_file_name    = (uint8_t*)&server_msg[3];

  for (int i = 0; i < NAME_LENGTH; i++) {
    msg_file_name[i] = filename[i];
  }

  Send(server_pID, server_msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, server_msg, SERVER_MSG_SIZE, 0);

  if (server_msg[0] < MIN_PID) {
    // the open file request failed.
    kprintf(" fail:");
    printx(server_msg[0], 4);
  } else {
    // success
    kprintf(" pID=");
    printx(server_msg[0], 4);
  }

  return server_msg[0];
}

int
StartThread(pID_t pID)
{
  server_msg[MSG_TYPE]    = MSG_NEW_THREAD;
  server_msg[MSG_USER_TYPE] = (uint32_t)pID;

  Send(server_pID, server_msg, SERVER_MSG_SIZE, 0);
  Receive(server_pID, server_msg, SERVER_MSG_SIZE, 0);

  if (server_msg[0] == OK) {
    kprintf(" OK");
    return OK;
  } else {
    kprintf(" Err: ");
    printx(server_msg[0], 4);
    return ERROR;
  }
}
// @A-

/* END OF FILE */
