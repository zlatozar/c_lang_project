///////////////////////////////////////////////////////////////////////////////////////////////////
/* Header for the Server
  server.h

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_H
#define SERVER_H

#include "../kernel/policy/kernel.h"
#include "../kernel/mechanism/pc/boot/bootdata.h"

#define NAME_LENGTH     24
#define BLOCKS_PER_PAGE   (PAGE_SIZE / BLOCK_SIZE)
#define BLOCK_ADDRESS_MASK  ((1 << BLOCK_SHIFT) - 1)
#define BLOCK_MASK      (~BLOCK_ADDRESS_MASK)

typedef int32_t       block_t;
typedef block_t       block_array[BLOCKS_PER_PAGE];

///////////////////////////////////////////////////////////////////////////////////////////////////
// Addresses in virtual and physical memory ///////////////////////////////////////////////////////

#define FIRST_USER_BLOCK  5
#define FS_LOWMEM     0x00010000
// file server's virt address where it starts mapping low 1MB phys.

#define NUM_FS_LOWMEM_PAGES 40
// NUM_FS_LOWMEM_PAGES * PAGE_SIZE = amount of low memory mapped to file server.

#define FS_BLOCK_BASE   (FS_LOWMEM + BOOT_SECTOR_PHYS)
//0x17C00 // file server's virt address for Block 0

#define FS_BOOT_TABLE   (FS_BLOCK_BASE + 4 * BLOCK_SIZE)
/* 0x18C00 // contains entries for the kernel, initial processes (Idle, Server, Disk Driver) and
 * the object table location. This boot table is 1KB in size, from 0x8C00 - 0x9000 (even though
 * only 5 entries are currently used).*/

#define FS_NEW_SCRATCH_PAGE 0x0005F000
#define FS_BOOT_PARAMS    (FS_BOOT_TABLE + BOOT_DATA_OFFSET)

///////////////////////////////////////////////////////////////////////////////////////////////////
// Thread Table Definition ////////////////////////////////////////////////////////////////////////

typedef struct thread_table_t thread_table_t;
struct thread_table_t {
  pID_t   pID;
  uint32_t  stack_bitmap;
  int     num_faults;
};

#define NUM_THREAD_ENTRIES  32
#define MAX_THREAD_PID    (NUM_THREAD_ENTRIES << VERSION_SHIFT) - 1

#define USER_STATUS_INIT  0x0400

// Server Message & Request Definitions
#define MSG_USER_TYPE   1
#define SERVER_MSG_SIZE   MSG_MAX_LENGTH

// Message/Request types. NB: this includes xxx_SYSCALL values in kernel.h
#define MSG_GET_NUM_PROC  0x100
#define MSG_INFO      0x133

#define MSG_NEW_PROCESS   0x131
#define MSG_NEW_THREAD    0x132
#define MSG_OPEN_INDEX    0x103
#define MSG_OPEN_NAME   0x104
#define MSG_CLOSE_PID   0x105
#define MSG_PAUSE_PID   0x114
#define MSG_RUN       0x115
#define MSG_RUN_PID     0x118
#define MSG_PRINT_NAME    0x119

#define MSG_SET_COORDINATOR 0x101
#define MSG_GET_COORDINATOR 0x102
#define MSG_CHECK_PAGE    0x117

#define MSG_BLOCK_READ    0x128
#define MSG_BLOCK_WRITE   0x129
#define MSG_BLOCK_OK    0x12A

#define MSG_FILE_STATUS   0x130

// Server Reason codes
#define REASON_SERVER_BLOCKED 0xAB

// Server Error Messages, kernel last is currently -0x7F check kernel.h
#define ERROR_FILE_OPEN     ERROR_KERNEL_LAST - 1
#define ERROR_FILE_NOT_OPEN   ERROR_KERNEL_LAST - 2
#define ERROR_FILE_NOT_FOUND  ERROR_KERNEL_LAST - 3

// Message & Request Definitions
#define MSG_OPEN        0x120
#define MSG_CLOSE       0x121
#define MSG_CREATE        0x122
#define MSG_DESTROY       0x123
#define MSG_GET         0x124
#define MSG_SET         0x125
#define MSG_VIDEO_MSG     0x126
#define MSG_VIDEO_READY     0x127

// User Reason values
#define USER_REASON_1     (REASON_KERNEL_LAST + 1)

#endif /* SERVER_H */
