///////////////////////////////////////////////////////////////////////////////////////////////////
/* Resource Mechanism defintions for PC
  resources_pc.h
  This file defines values for settings in the:
  1. programmable interrupt controller (PIC)
  2. clock device

  This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
  International License. To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-nd/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  Also see http://www.bluekernel.com.au

  copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __RESOURCES_PC_H
#define __RESOURCES_PC_H

#include "../../policy/types.h"
#include "../../policy/kernel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Resource Mechanism Global Data //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Resource Mechanism Definitions //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///// Interrupt Numbers ///////////////////////////////////////////////////////////////////////////

// Buffer is: MSG_TYPE|EXC_NO|PADDING|CR2|ERROR_CODE|EIP|CS|EFLAGS|ESP|SS
// these values can be used as indices into exception message buffer
//#define MSG_TYPE        0 // defined in kernel.h
#define EXC_NO          1
#define PADDING         2
/* This is to make the message 10 words. Padding is here instead of at the end because of the way
 * the exception stack is automatically pushed by the CPU.*/

#define CR2           3
#define ERROR_CODE        4
#define EIP           5
#define CS            6
#define EFLAGS          7
#define ESP           8
#define SS            9

//offset of hardware interrupts.
#define HWINT0          (INTERRUPT_OFFSET)
#define HWINT1          (INTERRUPT_OFFSET + 1)
#define HWINT2          (INTERRUPT_OFFSET + 2)
#define HWINT3          (INTERRUPT_OFFSET + 3)
#define HWINT4          (INTERRUPT_OFFSET + 4)
#define HWINT5          (INTERRUPT_OFFSET + 5)
#define HWINT6          (INTERRUPT_OFFSET + 6)
#define HWINT7          (INTERRUPT_OFFSET + 7)
#define HWINT8          (INTERRUPT_OFFSET + 8)
#define HWINT9          (INTERRUPT_OFFSET + 9)
#define HWINT10         (INTERRUPT_OFFSET + 10)
#define HWINT11         (INTERRUPT_OFFSET + 11)
#define HWINT12         (INTERRUPT_OFFSET + 12)
#define HWINT13         (INTERRUPT_OFFSET + 13)
#define HWINT14         (INTERRUPT_OFFSET + 14)
#define HWINT15         (INTERRUPT_OFFSET + 15)
#define HWINT_FIRST       HWINT0
#define HWINT_LAST        HWINT15
#define CLOCK_INTERRUPT     HWINT0
#define IRQ0          0
// IRQ0 is the clock.

#define EXCEPTION_OFFSET    (HWINT_LAST + 1)
#define EXCEPTION_FIRST     (EXCEPTION_OFFSET)
#define EXCEPTION_LAST      (EXCEPTION_OFFSET + 20)
#define PAGER_EXCEPTION     (EXCEPTION_OFFSET + 0x0E)
#define EXCEPTION_STACK_SIZE  10

///// Interrupt Controller ////////////////////////////////////////////////////////////////////////

#define PIC1          0x20
#define PIC2          0xA0
#define ICW1          0x11
//D4=1, IC4=1

#define ICW4          0x01
//0x01, PM=8086
//not-SFNM, non-buf,normal EOI

#define PIC1_COMMAND      PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND      PIC2
#define PIC2_DATA       (PIC2+1)
#define PIC_EOI         0x20
#define PIC1_START        0x20
#define PIC2_START        0x28

#define NOT_SFNM        0x00
#define NONBUF          0x00
#define BUFFERED        0x80
#define NORM_EOI        0x00
#define AUTO_EOI        0x02
#define PM8086          0x01
#define PM8085          0x00
#define ICW4_SLAVE        0x00
#define ICW4_MASTER       0x04

#define IC4           0x01
//ICW4 (not) needed

#define ICW1_TSELF        0x02
//Single (cascade) mode

#define ICW1_INTERVAL4      0x04
//Call address interval 4 (8)
#define ICW1_LEVEL        0x08
//Level triggered (edge) mode
#define ICW1_INIT       0x10
//Initialization

#define ICW4_8086       0x01
//8086/88 (MCS-80/85) mode

#define ICW4_AUTO       0x02
//Auto (normal) EOI

#define ICW4_BUF_SLAVE      0x08
//Buffered mode/slave

#define ICW4_BUF_MASTER     0x0C
//Buffered mode/master

#define ICW4_SFNM       0x10
//Special fully nested (not)

/*Use DATA port number to read OCW1 (Mask Register).
Use COMMAND port to use OCW3 to read ISR or IRR. */

#define OCW2          0x00
//bits D3/4 are 0, A0=0.

#define OCW3          0x08
//bit D4=0, D3=1, A0=0.

#define NOT_MASK_MODE     0x00
#define READ_ISR        0x0B
#define READ_IRR        0x0A
// end Interrupt Controller.

///// Clock Device ////////////////////////////////////////////////////////////////////////////////

#define TIMER_FREQ        1193182
//usually 1193182L Hz (1.19MHz) clock frequency for timer in PC and AT

#define TICK_FREQ       1000
// using 1000 means the interrupt should happen at 1kHz, or every 1ms.

#define TIMER_COUNT       ((uint8_t)(TIMER_FREQ / TICK_FREQ))
//initial value for counter, sent by InitClock()

#define SCHEDULE_RATE     50
// ~= 0.1sec, in number of ticks. SCHEDULE_RATE/TICK_FREQ = schedule frequency,
// eg 100/1000 = 0.1sec.

#define HZ            TICK_FREQ
// was set to 60, clock freq (software settable on IBM-PC) not sure about this value.

#define TIMER0          0x40
//I/O port for timer channel 0

//#define TIMER2        0x42
//I/O port for timer channel 2, not used in kernel.

#define TIMER_MODE        0x43
//I/O port for timer mode control

#define SQUARE_WAVE       0x36
//ccaammmb, a = access, m = mode, b = BCD
/*   00110110, cc=0, a=11 = LSB then MSB, 011 = sq wave, not BCD */

//#define COUNTER_FREQ      (20*TIMER_FREQ)
//counter frequency using square wave
//#define LATCH_COUNT     0x00
//cc00xxxx, c = channel, x = any
//#define CLOCK_ACK_BIT     0x80
//PS/2 clock interrupt acknowledge bit

#endif

/* END OF FILE */
