///////////////////////////////////////////////////////////////////////////////////////////////////
/* Disk Driver
  disk.c

  This disk driver is heavily based on the Minix2 driver. It has been stripped back as well as
  modified to work with this kernel. The relevant source file in Minix2 is src/kernel/at_wini.c
  and its associated files. It was written by Adri Koppes, with changes made by Kees J. Bot on
  13 April 1992.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "disk.h"
#include "../lib/debug.h"
#include "../lib/syscall.h"
#include "../lib/stdlib.h"
#include "../lib/syscallext.h"
#include "../../server/server.h"

int  kmain(void);

int  Open(void); // calls the init functions below
void InitParams(void); // init wini table parameters
void InitBuffer(void); // reserve memory for read/write buffer (add page).
void RegisterPorts(void); // register disk interrupts/ports in kernel with this process.
int  Identify(void);

int  DoRequest(void);

int  ReadWrite(int opcode, int sector, uint16_t* buffer);
int  ReadWriteDisk(int opcode, uint16_t* buffer, int num_sectors);
int  FlushCache(void);
int  DoTransfer(drive_t* drive_ptr, unsigned int precomp, unsigned int count, unsigned int sector,
                unsigned int opcode);
int  CommandOut(command_t* cmd);
int  CommandSimple(command_t* cmd);

void InitDrive(drive_t* drive_ptr, int base_cmd, int base_ctl, int irq, int ack, int hook,
               int drive);
void ControllerNeedsReset(void);
int  WaitForBusyErrorWriteFaultCleared(void);
void WaitForBusyCleared(void);
int  WaitForStatus(int mask, int value);
int  GetUptime(uint32_t* t);

int  Specify(void);
void RegisterInterrupt(void);
int  Test(void);

uint32_t  request[SERVER_MSG_SIZE];
uint32_t  reply[SERVER_MSG_SIZE];
pID_t   request_pID;
pID_t   server_pID, video_pID;
uint16_t*  disk_buffer;
int     init_done = 0;
int     test_status = 0;

int
kmain(void)
{
  Open();

  while (1) {
    request_pID = Receive(ANY, request, SERVER_MSG_SIZE, 0);
    DoRequest();
  }
}


int
Open(void)
{
  server_pID = GetCoordinator();

  InitParams();
  InitBuffer();
  RegisterPorts();

  /* assume first message to driver task is MSG_OPEN message. In Minix, this includes prepare,
   * identify, test, partition. These are all here in Identify():*/
  Identify();

  request[MSG_TYPE]   = MSG_SET;
  request[MSG_USER_TYPE]  = MSG_FILE_STATUS;
  request[2]        = USER_STATUS_INIT;
  Send(server_pID, request, SERVER_MSG_SIZE, 0);

  init_done = 1;

  return OK;
}

/* InitParams() initialises the drive table parameters - this is done for each array element in
 * driveT. The BIOS params are only filled in for existing drives, according to the BIOS num drives
 * param. The non-bios params are filled in for every drive record.*/
void
InitParams(void)
{
  // bios params
  /* bochs test disk: ata0-master: type=disk, path="c.img", mode=flat, cylinders=20, heads=16,
   * spt=63.*/
  drive_t* drive      = &driveT[0];
  drive->lcylinders   = 20; //bp_cylinders(params);
  drive->lheads     = 16; //bp_heads(params);
  drive->lsectors     = 63; //bp_sectors(params);
  drive->precomp      = 0; //bp_precomp(params) >> 2;

  // non-bios params
  int drive_num = 0; // my hardcode, just use first drive.

  InitDrive(drive, PORT_CMD, PORT_CTL, NO_IRQ, 0, 0, drive_num);

  // init params for PCI?

  drive->debug = 0x12345678;
}

void
InitDrive(drive_t* drive, int base_cmd, int base_ctl, int irq, int ack, int hook, int drive_num)
{
  drive->status   = 0;
  drive->w_status   = 0;
  drive->base_cmd   = base_cmd;
  drive->base_ctl   = base_ctl;
  drive->irq      = irq;
  drive->irq_mask   = 1 << irq;
  drive->irq_need_ack = ack;
  drive->irq_hook_id  = hook;
  drive->ldhpref    = ldh_init(drive_num);
  drive->max_count  = MAX_SECS << SECTOR_SHIFT;
  drive->lba48    = 0;
}

/* InitBuffer() reserves memory for read/write buffer by adding a page to the driver's memory.*/
void
InitBuffer(void)
{
  /* Makes sure physical memory is not on a 64KB boundary. We don't have that problem with
   * virtual memory, just put it at the start of a page.*/

  request[0] = ADD_PAGE_SYSCALL;
  request[1] = USE_CURRENT_PROCESS; //pID;
  request[2] = (uint32_t)DISK_BUFFER_ADDR; //virtual_address;
  request[3] = USER_RW; //permissions;
  Send(server_pID, request, SERVER_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);

  // the following initialisation is for debugging purposes.
  uint16_t* temp = DISK_BUFFER_ADDR;

  for (int i = 0; i < 256; i++)
  { *temp++ = i + 1; }

  disk_buffer = DISK_BUFFER_ADDR;

  return;
}

/* Register disk read/write ports in the kernel.*/
void
RegisterPorts(void)
{
  int i;

  for (i = 0; i < 8; i++) {
    request[0] = ADD_RESOURCE_SYSCALL;  //request type
    request[2] = IOPORT_RESOURCE;
    request[1] = USE_CURRENT_PROCESS;
    request[3] = PORT_CMD + i;      //resource number
    request[4] = REG_CMD_BASE0 + i;   // resource start
    Send(server_pID, request, SERVER_MSG_SIZE, 0);
    Receive(server_pID, reply, SERVER_MSG_SIZE, 0);
  }

  request[0] = ADD_RESOURCE_SYSCALL;    //request type
  request[2] = IOPORT_RESOURCE;
  request[1] = USE_CURRENT_PROCESS;
  request[3] = PORT_CMD + i;        // resource number.
  request[4] = REG_CMD_BASE0 + REG_CTL; // resource start.
  Send(server_pID, request, SERVER_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);
}

int
Identify(void)
{
  command_t cmd;
  drive_t* drive_ptr = &driveT[0];

  /* Try to identify the device. */
  cmd.ldh     = drive_ptr->ldhpref;
  cmd.precomp = cmd.count = cmd.sector = cmd.cyl_hi = cmd.cyl_lo = 0;
  cmd.command = ATA_IDENTIFY;

  // CommandSimple is CommandOut() and WaitForBusyErrorWriteFaultCleared():
  if (CommandSimple(&cmd) == OK) {
    /* This is an ATA device. */
    drive_ptr->status |= SMART;

    // get device info using Read16 from REG_DATA into buffer, of sector size.
    // From this device info, can fill in the drive values.

    ReadWriteDisk(CMD_READ, disk_buffer, 1); // 1 sector.

    // set CHS mode values.
    drive_ptr->pcylinders = disk_buffer[1];
    drive_ptr->pheads   = disk_buffer[3];
    drive_ptr->psectors   = disk_buffer[6];
  }
  //else ; // not OK, not an ATA device - don't use it unless BIOS params are known.

  /* Reset/calibrate (where necessary) */
  Specify();

  RegisterInterrupt();

  if (Test() == OK)
  { test_status = 1; }
  else
  { test_status = 0; }

  drive_ptr->status |= IDENTIFIED;

  return OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Request Handling ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int
DoRequest(void)
{
  uint32_t sector   = request[1] * 2;
  uint16_t* buffer  = (uint16_t*)request[2];

  switch (request[MSG_TYPE]) {
    case MSG_BLOCK_READ : {
      ReadWrite(CMD_READ, sector,   buffer);
      ReadWrite(CMD_READ, sector + 1, (buffer + (SECTOR_SIZE / 2)));

      request[MSG_TYPE] = MSG_BLOCK_OK;

      Send(request_pID, request, SERVER_MSG_SIZE, 0);

      break;
    }
    case MSG_BLOCK_WRITE : {
      ReadWrite(CMD_WRITE, sector,    buffer);
      ReadWrite(CMD_WRITE, sector + 1,  (buffer + (SECTOR_SIZE / 2)));

      request[MSG_TYPE] = MSG_BLOCK_OK;

      Send(request_pID, request, SERVER_MSG_SIZE, 0);

      break;
    }
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////
// Disk Functions //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int
ReadWrite(int opcode, int sector, uint16_t* buffer)
{
  drive_t* drive_ptr  = &driveT[0];
  int sec_count   = 1;

  int result      = DoTransfer(drive_ptr, drive_ptr->precomp, sec_count, sector, opcode);

  // Read Interrupt
  if (opcode == CMD_READ) {
    if ((result = WaitForBusyErrorWriteFaultCleared()) != OK) {
      //kprintf("Read Int error.");
      return ERR;
    }
  }

  if (!WaitForStatus(STATUS_DRQ, STATUS_DRQ)) {
    //kprintf("R/W DRQ error.");
    return ERR;
  }

  // Copy bytes
  disk_buffer = buffer;
  result    = ReadWriteDisk(opcode, disk_buffer, sec_count);

  return result;
}

int
ReadWriteDisk(int opcode, uint16_t* buffer, int num_sectors)
{
  // Minix only does a single sector for each of these. The interrupt
  // and polling tests are required again for each sector.
  drive_t* drive_ptr  = &driveT[0];
  int result      = 1;
  int num_words   = (num_sectors * SECTOR_SIZE / 2);

  int i = 0;

  while (i < num_words && (result > 0)) {
    if (opcode == CMD_READ) {
      result = Read16(drive_ptr->base_cmd + REG_DATA, buffer);
      buffer++;
    } else { // WRITE
      result = Write16(drive_ptr->base_cmd + REG_DATA, *buffer++);
    }
    i++;
  }

  if (result < 1) {
    //kprintf("R/W error");
  } else {
    if (opcode == CMD_READ) {
      //kprintf("Read OK.");
    } else {
      FlushCache();
      //kprintf("Write OK.");
    }
  }

  return result;
}

int
FlushCache(void)
{
  command_t cmd;
  cmd.ldh     = driveT[0].ldhpref;
  //cmd.precomp = cmd.count = cmd.sector = cmd.cyl_hi = cmd.cyl_lo = 0;
  cmd.command = CMD_CACHE_FLUSH;

  if (CommandSimple(&cmd) ==
      OK) { // CommandSimple is CommandOut and WaitForBusyErrorWriteFaultCleared():
    //kprintf("Flush!");
  } else
    ; //kprintf("No Flush.");

  return 1;
}

int
DoTransfer(drive_t* drive_ptr, unsigned int precomp, unsigned int count, unsigned int sector,
           unsigned int opcode)
{
  command_t cmd;
  unsigned secspcyl = drive_ptr->pheads * drive_ptr->psectors;

  cmd.precomp     = precomp;
  cmd.count     = count;
  cmd.command     = opcode;

  int cylinder    = sector / secspcyl;
  int head      = (sector % secspcyl) / drive_ptr->psectors;
  int sec       = sector % drive_ptr->psectors;
  cmd.sector      = sec + 1;
  cmd.cyl_lo      = cylinder & 0xFF;
  cmd.cyl_hi      = (cylinder >> 8) & 0xFF;
  cmd.ldh       = drive_ptr->ldhpref | head;

  return CommandOut(&cmd);
}
////////////////////////////////////////////////////////////////////////
// Command Output to Ports /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/* Output the command block to the winchester controller and return status */
int
CommandOut(command_t* cmd)
{
  drive_t* drive_ptr  = &driveT[0];
  unsigned base_cmd = drive_ptr->base_cmd;
  unsigned base_ctl = drive_ptr->base_ctl;

  if (!WaitForStatus(STATUS_BSY, 0)) {
    //printf("%s: controller not ready\n", w_name());
    //kprintf("Controller not ready,");
    return (ERR);
  }

  Write8(base_cmd + REG_LDH, cmd->ldh); // Select drive.

  if (!WaitForStatus(STATUS_BSY, 0)) {
    //printf("%s: CommandOut: drive not ready\n", w_name());
    //kprintf("CommandOut: drive not ready.");
    return (ERR);
  }

  //wn->w_status = STATUS_ADMBSY;
  w_command = cmd->command;

  // Note that base_ctl/base_cmd are kernel resource numbers, not raw PC ports.
  Write8(base_ctl,        drive_ptr->pheads >= 8 ? CTL_EIGHTHEADS : 0);
  Write8(base_cmd + REG_PRECOMP,  cmd->precomp);
  Write8(base_cmd + REG_COUNT,  cmd->count);
  Write8(base_cmd + REG_SECTOR, cmd->sector);
  Write8(base_cmd + REG_CYL_LO, cmd->cyl_lo);
  Write8(base_cmd + REG_CYL_HI, cmd->cyl_hi);
  Write8(base_cmd + REG_COMMAND,  cmd->command);

  return OK;
}

/* A simple controller command, only one interrupt and no data-out phase. */
int
CommandSimple(struct command_t* cmd)
{
  int result;
  drive_t* drive_ptr = &driveT[0];

  if ((result = CommandOut(cmd)) == OK)
  { result = WaitForBusyErrorWriteFaultCleared(); }

  w_command = CMD_IDLE;
  //kprintf("com simple result=");
  //printx(result,4);
  //printc('.');

  return result;
}

void
ControllerNeedsReset(void)
{
  drive_t* drive_ptr  = &driveT[0];

  drive_ptr->status |= DEAF;
  drive_ptr->status &= ~INITIALIZED;
}

/* Wait for an interrupt, study the status bits and return error/success.*/
int
WaitForBusyErrorWriteFaultCleared(void)
{
  int   result;
  int8_t  status;

  drive_t* drive_ptr = &driveT[0];

  WaitForBusyCleared(); //after waiting, BSY, WF, ERR should be clear.

  if ((drive_ptr->w_status & (STATUS_BSY | STATUS_WF | STATUS_ERR)) == 0) {
    result = OK;
  } else {
    int result = Read8(drive_ptr->base_cmd + REG_ERROR, &status);

    if (result != OK) {
      //kprintf("Couldn't read register");
      //panic(w_name(),"Couldn't read register",s);
    }

    if ((drive_ptr->w_status & STATUS_ERR) && (status & ERROR_BB))
    { result = ERR_BAD_SECTOR; }  /* sector marked bad, retries won't help */
    else
    { result = ERR; }       /* any other error */
  }

  return result;
}

/* Wait for a task completion interrupt. */
void
WaitForBusyCleared(void)
{
  /* W_INTR_WAIT: Checks if drive irq is set, if so, wait for interrupt with timeout. Copy device
   * status to drive_ptr->status using Read8(). Check status is STATUS_ADMBSY|STATUS_BSY and keep
   * waiting on interrupt until this is not true (ie device is ready).*/

  uint32_t msg[SERVER_MSG_SIZE];
  int result;
  drive_t* drive_ptr = &driveT[0];

  if (drive_ptr->irq != NO_IRQ) { // if IRQ set
    /* Wait for an interrupt that sets w_status to "not busy". */
    while (drive_ptr->w_status & (STATUS_ADMBSY | STATUS_BSY)) {
      result = Receive(PRI_DISK_INT, msg, SERVER_MSG_SIZE, wakeup_ticks); // timeout value?
      // disable checks for interrupt rather than timeout, just to see if R/W is working.

      if (result == PRI_DISK_INT) {
        Read8(drive_ptr->base_cmd + REG_STATUS, &drive_ptr->w_status);
      } else { // assume timeout, although may be another error.
        //kprintf("Disk error waiting for interrupt");
        //printf("AT_WINI got unexpected message %d from %d\n",m.m_type, m.m_source);
        //w_timeout();    /* a.o. set w_status */
      }
    }
  } else {
    /* Interrupt not yet allocated; use polling. */
    (void) WaitForStatus(STATUS_BSY, 0);
  }
}

/* Wait until controller is in the required state.  Return zero on timeout. An alarm that set a
 * timeout flag is used. TIMEOUT is in micros, we need ticks. Disabling the alarm is not needed,
 * because a static flag is used and a leftover timeout cannot do any harm.*/
int
WaitForStatus(int mask, int value)
{
  //clock_t t0, t1;
  uint32_t t0, t1;
  int s;
  drive_t* drive_ptr = &driveT[0];

  GetUptime(&t0);
  ////kprintf("t0:"); //printx(t1,4);  //printc('.');

  while ((s = GetUptime(&t1)) == OK && (t1 - t0) < timeout_ticks) {
    if ((s = Read8(drive_ptr->base_cmd + REG_STATUS, &drive_ptr->w_status)) != OK)
      ;
    //kprintf("Couldn't read register.");
    //panic(w_name(),"Couldn't read register",s);

    if ((drive_ptr->w_status & mask) == value) {
      ////kprintf("t1:"); //printx(t1,4);  //printc('.');

      return 1;
    }
  }

  if (s != OK) {
    //kprintf("AT_WINI: warning, get_uptime failed.");
    //printf("AT_WINI: warning, get_uptime failed: %d\n",s);
    ControllerNeedsReset();     /* controller gone deaf */
  }

  return 0;
}

int
GetUptime(uint32_t* t)
{
  *t = GetTime();
  return OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Init Functions /////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/* Initialise the drive with new parameters.*/
int
Specify(void)
{
  command_t cmd;
  drive_t* drive_ptr = &driveT[0];

  /* if ((wn->status & DEAF) && w_reset() != OK)
    return ERR; // shouldn't be deaf.*/

  if (!(drive_ptr->status & ATAPI)) {
    /* Specify parameters: precompensation, number of heads and sectors. */
    cmd.precomp = drive_ptr->precomp;
    cmd.count   = drive_ptr->psectors;
    cmd.ldh     = drive_ptr->ldhpref | (drive_ptr->pheads - 1);
    cmd.command = CMD_SPECIFY;    /* Specify some parameters */

    /* Output command block and see if controller accepts the parameters. */
    if (CommandSimple(&cmd) != OK)
    { return ERR; }

    if (!(drive_ptr->status & SMART)) {
      /* Calibrate an old disk. */
      cmd.sector  = 0;
      cmd.cyl_lo  = 0;
      cmd.cyl_hi  = 0;
      cmd.ldh     = drive_ptr->ldhpref;
      cmd.command = CMD_RECALIBRATE;

      if (CommandSimple(&cmd) != OK)
      { return (ERR); }
    }
  }

  drive_ptr->status |= INITIALIZED;

  return OK;
}

void
RegisterInterrupt(void)
{
  drive_t* drive_ptr = &driveT[0];

  if (drive_ptr->irq == NO_IRQ) {
    drive_ptr->irq      = PRI_DISK_INT; //w_drive < 2 ? AT_WINI_0_IRQ : AT_WINI_1_IRQ;
    drive_ptr->irq_hook_id  = drive_ptr->irq; /* id to be returned if interrupt occurs */
  }

  request[0] = ADD_RESOURCE_SYSCALL; //request type
  request[1] = USE_CURRENT_PROCESS;
  request[2] = INT_RESOURCE; //res_type
  request[3] = PRI_DISK_INT;
  Send(server_pID, request, SERVER_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);

  // Need to check reply value from system call, ie AddResource.
}

// Test is optionally called in Identify().
int
Test(void)
{
  ReadWrite(CMD_READ, 1024, DISK_BUFFER_ADDR); // sector 1024, at 512KiB mark on disk.

  // 5MiB mark on disk, is sector 5*1024*1024/512 = 10240. So read sector 10241.
  ReadWrite(CMD_READ, 5 * MB / SECTOR_SIZE + 1, DISK_BUFFER_ADDR);

  ReadWrite(CMD_READ, 0, DISK_BUFFER_ADDR);

  char test_string[]      = "Test the disk is working okay.";
  char* test_string_to_write  = (char*)DISK_BUFFER_ADDR;

  /* Fill the disk buffer with zeroes. This will also ensure our string is null terminated (which
   * kstrcpy() won't write, but kstrcmp terminates on null.*/
  for (int i = 0; i < PAGE_SIZE; i++)
  { *test_string_to_write++ = (char)0; }

  // reset the address of our target test string to the start of the buffer.
  test_string_to_write = (char*)DISK_BUFFER_ADDR;

  kstrcpy(test_string_to_write, test_string);

  /* Write the buffer to the disk at sector 212. Then read back sector 5, followed by sector 212
   * to ensure the buffer then has our string. Hopefully sector 212 does not contain any actual
   * file data.*/
  ReadWrite(CMD_WRITE,  212,  DISK_BUFFER_ADDR);
  ReadWrite(CMD_READ,   5,    DISK_BUFFER_ADDR);
  ReadWrite(CMD_READ,   212,  DISK_BUFFER_ADDR);

  if (kstrcmp(test_string_to_write, test_string) != 0)
  { return ERR; }
  else
  { return OK; }
}

/* END OF FILE */
