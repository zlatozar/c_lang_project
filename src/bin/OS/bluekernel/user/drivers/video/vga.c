/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "vga.h"

#define BGA_ENABLE 1

#if BGA_ENABLE
int
TestBGA(void)
{
  // Test Bochs VBE
  // Register ports
#define BGA_INDEX (0x20 + PORT_PID_FIRST)
#define BGA_DATA  (BGA_INDEX + 1)
  request[0] = ADD_RESOURCE_SYSCALL; //request type
  request[2] = IOPORT_RESOURCE; //res_type
  request[1] = USE_CURRENT_PROCESS; //res_pID
  request[3] = BGA_INDEX; //resource number
  request[4] = VBE_DISPI_IOPORT_INDEX; //port number
  Send(server_pID, request, COORD_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);
  request[3] = BGA_DATA; //resource number
  request[4] = VBE_DISPI_IOPORT_DATA; //port number
  Send(server_pID, request, COORD_MSG_SIZE, 0);
  Receive(server_pID, reply, SERVER_MSG_SIZE, 0);

  Write16(BGA_INDEX, VBE_DISPI_INDEX_ID);
  int index = 0;
  Read16(BGA_DATA, &index); // this returns 0xB0C5

  /* In order to change the contents of registers 1-3 (VBE_DISPI_INDEX_XRES,
   * VBE_DISPI_INDEX_YRES, VBE_DISPI_INDEX_BPP) the VBE extensions must be disabled first. To do
   * so, write the value VBE_DISPI_DISABLED (0x00) to VBE_DISPI_INDEX_ENABLE (4). The changes are
   * not visible until the VBE extensions are enabled again. To do so, write the value
   * VBE_DISPI_ENABLED (0x01) to the same register.*/
  Write16(BGA_INDEX, VBE_DISPI_INDEX_ENABLE); //select
  Write16(BGA_DATA, VBE_DISPI_DISABLED); // write - disable VBE

  /* only 320x200, 640x400, 640x480, 800x600, 1024x768 are currently supported, according to
   * old note on website.*/
  Write16(BGA_INDEX, VBE_DISPI_INDEX_XRES); //select
  Write16(BGA_DATA, 800); // write - disable VBE
  Write16(BGA_INDEX, VBE_DISPI_INDEX_YRES); //select
  Write16(BGA_DATA, 600); // write - disable VBE
  Write16(BGA_INDEX, VBE_DISPI_INDEX_BPP); //select
  Write16(BGA_DATA, VBE_DISPI_BPP_32); // write - disable VBE

  Write16(BGA_INDEX, VBE_DISPI_INDEX_ENABLE); //select
  Write16(BGA_DATA, VBE_DISPI_ENABLED /*| VBE_DISPI_NOCLEARMEM*/); // write - disable VBE
  return 1;
}

void
Draw(void)
{
  uint32_t* pixel = (uint32_t*)VIDEO_BASE;
  int i;
  BREAK;
  for (i = 0; i < 1600; i++) { *pixel++ = 0x000000BB; }

}
#endif

