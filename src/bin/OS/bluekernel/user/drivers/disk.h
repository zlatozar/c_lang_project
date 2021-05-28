///////////////////////////////////////////////////////////////////////////////////////////////////
/* Disk Driver Header
	disk.c

	This disk driver is heavily based on the Minix2 driver. It has been stripped back as well as
	modified to work with this kernel. The relevant source file in Minix2 is src/kernel/at_wini.c
	and its associated files. It was written by Adri Koppes, with changes made by Kees J. Bot on
	13 April 1992.

	Bochs test disk is 10MB and params are:
	ata0-master: type=disk, path="c.img", mode=flat, cylinders=20, heads=16, spt=63

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DISK_H
#define DISK_H

#include "../../kernel/policy/kernel.h"

#define PORT_CMD			(0x10 + PORT_PID_FIRST)
#define PORT_CTL			(PORT_CMD + 8)
#define PRI_DISK_INT		HWINT14
#define SEC_DISK_INT		HWINT15

#define DISK_BUFFER_ADDR	(uint16_t*)0x5000
// disk data buffer for read/write.

///////////////////////////////////////////////////////////////////////////////////////////////////
// Minix 2 definitions ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define ATAPI_DEBUG			0		/* To debug ATAPI code. */

/* I/O Ports used by winchester disk controllers. */

/* Read and write registers */
#define REG_CMD_BASE0		0x1F0	/* command base register of controller 0 */
#define REG_CMD_BASE1		0x170	/* command base register of controller 1 */
#define REG_CTL_BASE0		0x3F6	/* control base register of controller 0 */
#define REG_CTL_BASE1		0x376	/* control base register of controller 1 */

#define REG_DATA			0		/* data register (offset from the base reg.) */
#define REG_PRECOMP			1		/* start of write precompensation */
#define REG_COUNT			2		/* sectors to transfer */
#define REG_SECTOR			3		/* sector number */
#define REG_CYL_LO			4		/* low byte of cylinder number */
#define REG_CYL_HI			5		/* high byte of cylinder number */
#define REG_LDH				6		/* lba, drive and head */
#define	LDH_DEFAULT			0xA0	/* ECC enable, 512 bytes per sector */
#define	LDH_LBA				0x40	/* Use LBA addressing */
#define	ldh_init(drive_num)	(LDH_DEFAULT | ((drive_num) << 4))

/* Read only registers */
#define REG_STATUS			7		/* status */
#define	STATUS_BSY			0x80	/* controller busy */
#define	STATUS_RDY			0x40	/* drive ready */
#define	STATUS_WF			0x20	/* write fault */
#define	STATUS_SC			0x10	/* seek complete (obsolete) */
#define	STATUS_DRQ			0x08	/* data transfer request */
#define	STATUS_CRD			0x04	/* corrected data */
#define	STATUS_IDX			0x02	/* index pulse */
#define	STATUS_ERR			0x01	/* error */
#define	STATUS_ADMBSY		0x100	/* administratively busy (software) */

#define REG_ERROR			1		/* error code */
#define	ERROR_BB			0x80	/* bad block */
#define	ERROR_ECC			0x40	/* bad ecc bytes */
#define	ERROR_ID			0x10	/* id not found */
#define	ERROR_AC			0x04	/* aborted command */
#define	ERROR_TK			0x02	/* track zero error */
#define	ERROR_DM			0x01	/* no data address mark */

/* Write only registers */
#define REG_COMMAND			7		/* command */
#define	CMD_IDLE			0x00	/* for w_command: drive idle */
#define	CMD_RECALIBRATE		0x10	/* recalibrate drive */
#define	CMD_READ			0x20	/* read data */
#define	CMD_READ_EXT		0x24	/* read data (LBA48 addressed) */
#define	CMD_WRITE			0x30	/* write data */
#define	CMD_WRITE_EXT		0x34	/* write data (LBA48 addressed) */
#define	CMD_READVERIFY		0x40	/* read verify */
#define	CMD_FORMAT			0x50	/* format track */
#define	CMD_SEEK			0x70	/* seek cylinder */
#define	CMD_DIAG			0x90	/* execute device diagnostics */
#define	CMD_SPECIFY			0x91	/* specify parameters */

// Not included in Minix2 files.
#define CMD_CACHE_FLUSH		0xE7

#define	ATA_IDENTIFY		0xEC	/* identify drive */

#define REG_CTL				0x206	/* control register */
#define	CTL_NORETRY			0x80	/* disable access retry */
#define	CTL_NOECC			0x40	/* disable ecc retry */
#define	CTL_EIGHTHEADS		0x08	/* more than eight heads */
#define	CTL_RESET			0x04	/* reset controller */
#define	CTL_INTDISABLE		0x02	/* disable interrupts */

#define REG_STATUS			7		/* status */
#define	STATUS_BSY			0x80	/* controller busy */
#define	STATUS_DRDY			0x40	/* drive ready */
#define	STATUS_DMADF		0x20	/* dma ready/drive fault */
#define	STATUS_SRVCDSC		0x10	/* service or dsc */
#define	STATUS_DRQ			0x08	/* data transfer request */
#define	STATUS_CORR			0x04	/* correctable error occurred */
#define	STATUS_CHECK		0x01	/* check error */

/* Interrupt request lines. */
#define NO_IRQ				0		/* no IRQ set yet */

#define ATAPI_PACKETSIZE	12
#define SENSE_PACKETSIZE	18

/* Error codes */
#define ERR					(-1)	/* general error */
#define ERR_BAD_SECTOR		(-2)	/* block marked bad detected */

/* Some controllers don't interrupt, the clock will wake us up. */
#define WAKEUP				(32*HZ)	/* drive may be out for 31 seconds max */
	// HZ is defined in kernel.h

/* Miscellaneous. */
#define MAX_DRIVES			1
#define COMPAT_DRIVES		1
#define MAX_SECS			256		/* controller can transfer this many sectors */
#define MAX_ERRORS			4		/* how often to try rd/wt before quitting */
#define NR_MINORS			(MAX_DRIVES * DEV_PER_DRIVE)
#define SUB_PER_DRIVE		(NR_PARTITIONS * NR_PARTITIONS)
#define NR_SUBDEVS			(MAX_DRIVES * SUB_PER_DRIVE)
#define DELAY_USECS			1000	/* controller timeout in microseconds */
#define DELAY_TICKS 		1		/* controller timeout in ticks */
#define DEF_TIMEOUT_TICKS	5000	/* controller timeout in ticks */
// Think this should be ~5sec, ie 5000 (I think it was about 300).

#define RECOVERY_USECS		500000	/* controller recovery time in microseconds */
#define RECOVERY_TICKS		30		/* controller recovery time in ticks */
#define INITIALIZED			0x01	/* drive is initialized */
#define DEAF				0x02	/* controller must be reset */
#define SMART				0x04	/* drive supports ATA commands */
#define ATAPI				0		/* don't bother with ATAPI; optimise out */
#define IDENTIFIED			0x10	/* w_identify done successfully */
#define IGNORING			0x20	/* w_identify failed once */

// Data ///////////////////////////////////////////////////////////////////////////////////////////

/* Common command block */
typedef struct command_t
{
	uint8_t	precomp;	/* REG_PRECOMP, etc. */
	uint8_t	count;
	uint8_t	sector;
	uint8_t	cyl_lo;
	uint8_t	cyl_hi;
	uint8_t	ldh;
	uint8_t	command;
} command_t;

/* Timeouts and max retries. */
int timeout_ticks	= DEF_TIMEOUT_TICKS;
int max_errors		= MAX_ERRORS;
int wakeup_ticks	= WAKEUP; 

/* Variables. */

/* Main drive structure, one entry per drive.
 * The size of this table is 20 * 32-bit.
 * This structure is a simplified version of the 'wini' structure in Minix2.
 */
typedef struct drive_t				
{
  unsigned status;			/* drive state: deaf, initialized, dead */ // changed from 'state'.
  unsigned w_status;		/* device status register */
  unsigned base_cmd;		/* command base register */
  unsigned base_ctl;		/* control base register */

  unsigned irq;				/* interrupt request line */
  unsigned irq_mask;		/* 1 << irq */
  unsigned irq_need_ack;	/* irq needs to be acknowledged */
  int	   irq_hook_id;		/* id of irq hook at the kernel */

  int	   lba48;			/* supports lba48 */
  unsigned lcylinders;		/* logical number of cylinders (BIOS) */
  unsigned lheads;			/* logical number of heads */
  unsigned lsectors;		/* logical number of sectors per track */

  unsigned pcylinders;		/* physical number of cylinders (translated) */
  unsigned pheads;			/* physical number of heads */
  unsigned psectors;		/* physical number of sectors per track */
  unsigned ldhpref;			/* top four bytes of the LDH (head) register */

  unsigned precomp;			/* write precompensation cylinder / 4 */
  unsigned max_count;		/* max request for this drive */
  unsigned open_ct;			/* in-use count */
  //struct device part[DEV_PER_DRIVE];		/* disks and partitions */
  //struct device subpart[SUB_PER_DRIVE];	/* subpartitions */
  unsigned debug;			/* added by P.C.*/
} drive_t;
drive_t driveT[MAX_DRIVES];

int w_command;				/* current command in execution */

//// from driver.h in Minix
/* Parameters for the disk drive. */
#define SECTOR_SIZE		512		/* physical sector size in bytes */
#define SECTOR_SHIFT	9		/* for division */
#define SECTOR_MASK		511		/* and remainder */

#endif	// DISK_H

/* END OF FILE */
