///////////////////////////////////////////////////////////////////////////////////////////////////
/* Boot Data pointers

  This file defines boot sector data and the boot table data used to set parameters for
  initialising the kernel. Some values from the boot mechanisms are saved here too so that the
  kernel knows how the system was loaded.

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "bootdata.h"

////////////////////////////////////////////////////////////////////////////////
// Boot Sector Data ////////////////////////////////////////////////////////////

uint16_t* const sectors_required  = (uint16_t*)(BOOT_SECTOR_VIRT + SECTORS_REQD_OFFSET);
uint16_t* const sec_transferred   = (uint16_t*)(BOOT_SECTOR_VIRT + 0x7);

uint8_t* const  bootdrv       = (uint8_t*)(BOOT_SECTOR_VIRT + 0x09);
uint8_t* const  drivetype     = (uint8_t*)(BOOT_SECTOR_VIRT + 0x0A);
uint8_t* const  disktries     = (uint8_t*)(BOOT_SECTOR_VIRT + 0x0B);
uint8_t* const  num_drives    = (uint8_t*)(BOOT_SECTOR_VIRT + 0x0C);

uint8_t* const  max_sec       = (uint8_t*)(BOOT_SECTOR_VIRT + 0x0D);
uint8_t* const  max_head      = (uint8_t*)(BOOT_SECTOR_VIRT + 0x0E);
uint16_t* const max_cyl       = (uint16_t*)(BOOT_SECTOR_VIRT + 0x0F);

uint8_t* const  curr_sec      = (uint8_t*)(BOOT_SECTOR_VIRT + 0x10);
uint8_t* const  curr_head     = (uint8_t*)(BOOT_SECTOR_VIRT + 0x11);
uint16_t* const curr_cyl      = (uint16_t*)(BOOT_SECTOR_VIRT + 0x12);
uint16_t* const seg_start     = (uint16_t*)(BOOT_SECTOR_VIRT + 0x13);
uint16_t* const seg_end       = (uint16_t*)(BOOT_SECTOR_VIRT + 0x15);

uint8_t* const  num_sec       = (uint8_t*)(BOOT_SECTOR_VIRT + 0x17);

uint16_t* const diskmsg       = (uint16_t*)(BOOT_SECTOR_VIRT + 0x26);

////////////////////////////////////////////////////////////////////////////////
// Boot Table Data /////////////////////////////////////////////////////////////

uint32_t* const boot_user_params      = (uint32_t*)(BOOT_DATA_BASE + 0);
uint32_t* const boot_max_processes    = (uint32_t*)(BOOT_DATA_BASE + 4);
uint32_t* const boot_version_bits     = (uint32_t*)(BOOT_DATA_BASE + 8);
uint32_t* const boot_matching_bits    = (uint32_t*)(BOOT_DATA_BASE + 12);
uint32_t* const boot_transparent_bits = (uint32_t*)(BOOT_DATA_BASE + 16);
uint32_t* const boot_block_size       = (uint32_t*)(BOOT_DATA_BASE + 20);
uint32_t* const boot_num_ioports      = (uint32_t*)(BOOT_DATA_BASE + 24);
uint32_t* const boot_num_page_maps    = (uint32_t*)(BOOT_DATA_BASE + 28);

/* END OF FILE */
