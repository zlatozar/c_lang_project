///////////////////////////////////////////////////////////////////////////////////////////////////
/* Kernel mechanism interfaces
  mec.h

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MEC_H
#define MEC_H

extern int    InitialiseUsersMec(void);
extern void   InitBootParams(void);

extern int    CreateProcessMec(void);
extern int    CreateResourceMec(void);
extern uint32_t CreateMemoryMec(void);

extern void   LockBus(void);
extern void   UnlockBus(void);

extern void   SwitchProcessMec(process_table_t* pID_ptr);
extern int    AddProcessMec(process_table_t* pID_ptr);
extern uint32_t RemoveProcessMec(process_table_t* pID_ptr);

extern int    AddMemoryMec(process_table_t* pID_ptr);
extern int    AddPageMec(process_table_t* pID_ptr, int32_t physical_page, uint32_t virtual_address,
                         uint32_t permissions);
extern int32_t  RemovePageMec(process_table_t* pID_ptr, uint32_t virtual_address);
extern void   EditPageMec(uint32_t physical_page);
extern void   ClearPageMec(void);
extern uint32_t GetMemoryMec(process_table_t* pID_ptr, int property, uint32_t virtual_address);
extern uint32_t GetPhysAddrMec(process_table_t* pID_ptr, uint32_t virtual_address);
extern uint32_t GetPermissionsMec(process_table_t* pID_ptr, uint32_t virtual_address);

extern int    WritePort(uint32_t resource_num, uint32_t* data, int32_t port_type);
extern int    ReadPort(uint32_t resource_num, uint32_t* data, int32_t port_type);
extern int    EnableInterrupt(int hwintn);
extern uint8_t    DisableInterrupt(int hwintn);
extern int    ClockHandler(void);
extern uint32_t GetMemorySize(void);

#if VIDEO_DEBUG
extern int    VideoInit(void);
extern int    VideoChangeMode(void);
extern void   BgaWriteRegister(uint16_t index, uint16_t value);
extern uint16_t BgaReadRegister(uint16_t index);
extern int    BgaIsAvailable(void);

extern void   VideoProcessUpdate(void);
extern void   VideoExceptionOut(uint32_t exc_msg);
extern void   VideoAddPage(pID_t curr_pID, pID_t pID, uint32_t virtual_address, uint32_t getphys);
#endif

#endif

