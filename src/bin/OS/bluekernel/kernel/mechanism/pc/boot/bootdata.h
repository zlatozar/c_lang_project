///////////////////////////////////////////////////////////////////////////////////////////////////
/* Header for Boot Data pointers

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

#ifndef __BOOTDATA_H
#define __BOOTDATA_H

#include "../kernel_ia32.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Boot Sector Data
// ///////////////////////////////////////////////////////////////////////////////

extern uint16_t* const sectors_required;
extern uint16_t* const sec_transferred;

extern uint8_t*  const bootdrv;
extern uint8_t*  const drivetype;
extern uint8_t*  const disktries;
extern uint8_t*  const num_drives;

extern uint8_t*  const max_sec;
extern uint8_t*  const max_head;
extern uint16_t* const max_cyl;

extern uint8_t*  const curr_sec;
extern uint8_t*  const curr_head;
extern uint16_t* const curr_cyl;
extern uint16_t* const seg_start;
extern uint16_t* const seg_end;
extern uint8_t*  const num_sec;

extern uint16_t* const diskmsg;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Boot Table Data ////////////////////////////////////////////////////////////////////////////////

#define NUM_BOOT_PARAMS 8

extern uint32_t* const boot_user_params;
extern uint32_t* const boot_max_processes;
extern uint32_t* const boot_version_bits;
extern uint32_t* const boot_matching_bits;
extern uint32_t* const boot_transparent_bits;
extern uint32_t* const boot_block_size;
extern uint32_t* const boot_num_ioports;
extern uint32_t* const boot_num_page_maps;
#endif

/* END OF FILE */
