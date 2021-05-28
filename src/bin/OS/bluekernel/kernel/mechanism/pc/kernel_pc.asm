; -------------------------------------------------------------------------------------------------
; kernel_pc.asm
; This file contains support routines for the kernel C code.
;
;	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
;	International License. To view a copy of this license, visit
;	http://creativecommons.org/licenses/by-nc-nd/4.0/
;	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
;
;	Also see http://www.bluekernel.com.au
;
;	copyright Paul Cuttler 2017
; -------------------------------------------------------------------------------------------------

;; Documentation annotations:
;; @A web_mec_pc_intro_ia32 Kernel Mechanism Initialisation Code
;; @E web_mec_pc_resources_exceptions Resource Mechanism Exception Code
;; @T web_mec_pc_memory_tlb2 Flushing the TLB

;;;;;; Kernel low level mechanisms ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global LockBus,UnlockBus,ReloadGDT,LoadTaskRegister,Flush
global In8,Out8, In16, Out16, In32, Out32

;;;;;; Hardware interrupt handlers ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global HwInt0,HwInt1,HwInt2,HwInt3,HwInt4,HwInt5,HwInt6,HwInt7,HwInt8
global HwInt9,HwInt10,HwInt11,HwInt12,HwInt13,HwInt14,HwInt15

;;;;;; Exception handlers ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global DivideErrorExc, SingleStepExc, NmiExc, BpExc, OverflowExc, BoundsCheckExc
global InvalidOpcodeExc, CoCpuExc, DoubleFaultExc, CoCpuOverrunExc
global InvalidTssExc, SegNotPresentExc, StackExc, GenProtectionExc
global PageFaultExc, CoCpuErrorExc

;;;;;; Kernel policy-level functions: ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
extern Interrupt
extern Exception

;;;;;; Kernel policy-level functions for System Calls.
extern Send,Receive,ScheduleCall,AddProcessCall,RemoveProcessCall,AddThreadCall
extern RemoveThreadCall,GetProcessCall,SetProcessCall
extern AddResource,RemoveResource
;extern Read8,Write8,Read16,Write16,Read32,Write32
extern AddPageCall, MapPageCall, GrantPageCall, UnmapPageCall

;;;;;; Call Gate entry points for system calls  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global SendGate,ReceiveGate,ScheduleGate,AddProcessGate,RemoveProcessGate
global AddThreadGate,RemoveThreadGate,GetProcessGate, SetProcessGate
global AddResourceGate,RemoveResourceGate
;global Read8Gate,Write8Gate,Read16Gate, Write16Gate,Read32Gate,Write32Gate
global AddPageGate, MapPageGate, GrantPageGate, UnmapPageGate

;; @A+
; all the #define constants from C headers must be included here:
%include "kernel.inc"
%include "kernel_ia32.inc"
%include "resources_pc.inc"
	
segment .data ;
	lock_var		db 0
	gdt_limit		dw GDT_SIZE-1
	gdt_base		dd GDT_BASE
	idt_limit		dw IDT_SIZE-1
	idt_base		dd IDT_BASE

segment .text
;;;; CALL GATES ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Refer to comments for Send Gate to get an idea for other gates.

SendGate:
	push ebp
	mov ebp, esp
	; Need to set ds and es so that gate can access kernel data.
	push edx ; does this need saving?  depends on compiler perhaps.
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+24]
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12] ; parameters start at +12 for
		; gates because CS is pushed with EIP.
	call Send
		; return value in eax.
	; restore segments
	add esp,16 ; remove 4 args.
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 16 ;far return, inc sp by 4 for each parameter.
;; @A-

ReceiveGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+24]
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call Receive
	add esp,16
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 16

ScheduleGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	call ScheduleCall
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 0

AddProcessGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+36]
	push dword [ebp+32]
	push dword [ebp+28]
	push dword [ebp+24]
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call AddProcessCall
	add esp,28
	mov word dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 28

RemoveProcessGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+12]
	call RemoveProcessCall
	add esp,4
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 4

GetProcessGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+16]
	push dword [ebp+12]
	call GetProcessCall
	add esp,8
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	mov esp, ebp
	pop ebp
	retf 8

SetProcessGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call SetProcessCall
	add esp,12
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 12

AddResourceGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+24]
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call AddResource
	add esp,16
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 16

RemoveResourceGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+16]
	push dword [ebp+12]
	call RemoveResource
	add esp,8
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 8

AddPageGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call AddPageCall
	add esp,12
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 12

MapPageGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+28]
	push dword [ebp+24]
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call MapPageCall
	add esp,20
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 20

GrantPageGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+28]
	push dword [ebp+24]
	push dword [ebp+20]
	push dword [ebp+16]
	push dword [ebp+12]
	call GrantPageCall
	add esp,20
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 20

UnmapPageGate:
	push ebp
	mov ebp, esp
	push edx
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push dword [ebp+16]
	push dword [ebp+12]
	call UnmapPageCall
	add esp,8
	mov dx, DSu_SEL
	mov ds, dx
	mov es, dx
	pop edx
	mov esp, ebp
	pop ebp
	retf 8

;; @A+
; -----------------------------------------------------------------------------
; Following functions used in controlling interrupts, scheduling, etc...
; -----------------------------------------------------------------------------
LockBus:
; compare lock_var to al (0).  If compares, set lock_var to 1.
; 0 means lock available, 1 means locked.
	push eax
	push ebx
	mov al, 0
	mov bl, 1
  lock_loop:
	lock cmpxchg [lock_var], bl
	jnz lock_loop ; lock not granted.
	pop ebx
	pop eax
	cli
	ret ; lock granted.

UnlockBus:
	mov byte [lock_var], 0
	sti
	ret

ReloadGDT:
	lgdt [gdt_limit]
	lidt [idt_limit]
	ret

LoadTaskRegister:
; 	push ebp ; normal stack frame not used because function is simple.
; 	mov ebp, esp
	mov ax, [esp+4];[ebp + 8] ; Task Register selector
	ltr ax
; 	mov esp, ebp
; 	pop ebp
	ret

;; @T+	
Flush:
	mov eax, [esp+4]
	invlpg [eax]
	ret
;; @T-

In8:
	; u8_t In8(u32_t port);
	push edx
	mov edx, [esp+8] ; port --> replace the top 3 lines with this line.
	xor eax, eax ; clear eax for port input data.
	in al, dx
	pop edx
	ret

Out8:
	; u32_t Out8(u32_t port, u8_t data);
	push edx
	mov edx, [esp+8] ; port
	mov eax, [esp+12] ; data
	out dx, al
	pop edx
	ret
;; @A-

In16:
	; u16_t In16(u32_t port);
	push edx
	mov edx, [esp+8] ; port
	xor eax, eax ; clear eax for port input data.
	in ax, dx
	pop edx
	ret

Out16:
	; u32_t Out16(u32_t port, u16_t data);
	push edx
	mov edx, [esp+8] ; port
	mov eax, [esp+12] ; data
	out dx, ax
	pop edx
	ret

In32:
	; u32_t In32(u32_t port);
	push edx
	mov edx, [esp+8] ; port
	xor eax, eax ; clear eax for port input data.
	in eax, dx
	pop edx
	ret

Out32:
	; u32_t Out32(u32_t port, u32_t data);
	push edx
	mov edx, [esp+8] ; port
	mov eax, [esp+12] ; data
	out dx, eax
	pop edx
	ret

;; @A+
; -----------------------------------------------------------------------------
; Macros for hardware interrupts, which each jump to a different C code ISR.
; Hardware ISRs are divided between master and slave interrupts on the two
; i8259 Interrupt Controllers.
; The number after the macro name represents the number of parameters passed to
; the macro. This number is used to indicate the IRQn.
; -----------------------------------------------------------------------------
%macro hwint_master 1
	nop ; this is here because bochs debugger misses 1st instruction on interrupt
		; breakpoints.
	; save process state after interrupt.
	pushad ;PUSHAD Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI
	push ds
	push es
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	; for debugging, make this C code from here, where Interrupt() issues PIC commands:
	in al, PIC1_DATA ; mask data.
	or al, (1<<%1) ; [1<<irq] IRQn
	out PIC1_DATA, al ; disable IRQn
	mov al, PIC_EOI ; ack int code for PIC.  Clears highest IRQ from ISR.
	out PIC1_COMMAND, al ;acknowledge IRQ
	; this is end of asm section that has been shifted to C code.
	push %1 + INTERRUPT_OFFSET
	call Interrupt
	add esp, 4 ;remove arg to Interrupt
	pop es
	pop ds
	popad
	iret
%endmacro

%macro hwint_slave 1
	nop
	; save process state after interrupt.
	pushad
	push ds
	push es
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	in  al, PIC2_DATA ; mask data.
	or  al, (1<<(%1-8))
	out PIC2_DATA, al
	mov al, PIC_EOI
	out PIC1_COMMAND, al ;acknowledge master IRQ
	nop
	out PIC2_COMMAND, al ;ack slave.
	push %1 + INTERRUPT_OFFSET
	call Interrupt
	add esp, 4 ;remove arg to Interrupt
	pop es
	pop ds
	popad
	iret
%endmacro

;; @E+
; -----------------------------------------------------------------------------
%macro exception_isr 1
	; at this point, esp points to EIP on stack, with CS|EFLAGS|ESP|SS above it.
	push -1 ;error code. This makes all exceptions have an error code.
	mov eax, cr2
	push eax ; page fault address.
	push dword 0x12345678 ; padding
	push dword %1; exc no.
	push dword EXCEPTION_SYSCALL
	; esp now points to MSG_TYPE|EXC_NO|PADDING|CR2|ERROR|EIP|CS|EFLAGS|ESP|SS
	mov ecx, esp ; save stack position to pass as message buffer to Exception()
	pushad
	push ds
	push es
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push ecx ; exception's esp.
	push dword %1 + EXCEPTION_OFFSET; exc no.
	call Exception
	; Exception(u32_t exc_num, u32_t exc_msg);
	add esp,8 ; remove arguments to Exception()
	;;;;;
	pop es
	pop ds
	popad
	;;;
	add esp, 20 ; remove error code, CR2, exc no.
	iret ; remainder of exception stack is removed automatically by IRET
%endmacro

%macro exception_isr_error 1
	; NB: if exception is generated by sw int, then error code is not generated.
	; see IA-32 vol 3 section 5.1.2.2.
	mov eax, cr2
	push eax ; page fault address.
	push dword 0x12345678 ; padding
	push dword %1; exc no.
	push dword EXCEPTION_SYSCALL
	; esp now points to MSG_TYPE|EXC_NO|PADDING|CR2|ERROR_CODE|EIP|CS|EFLAGS|ESP|SS
	mov ecx, esp ; save stack position to pass as message buffer to Exception()
	pushad
	push ds
	push es
	mov dx, DSk_SEL
	mov ds, dx
	mov es, dx
	push ecx ; exception's esp.
	push dword %1 + EXCEPTION_OFFSET ; exc no.
	call Exception
 	add esp,8 ; 2 args to Exception.
	pop es
	pop ds
	popad
	add esp, 20 ; remove error code.; handler must remove error code from stack
	; before executing return. Refer to (s)5.11 of Intel #3.
	iret
%endmacro
;; @E-

HwInt0: hwint_master 0
HwInt1: hwint_master 1
;; @A-
HwInt2: hwint_master 2
HwInt3: hwint_master 3
HwInt4: hwint_master 4
HwInt5: hwint_master 5
HwInt6: hwint_master 6
HwInt7: hwint_master 7

HwInt8: hwint_slave 8
HwInt9: hwint_slave 9
HwInt10: hwint_slave 10
HwInt11: hwint_slave 11
HwInt12: hwint_slave 12
HwInt13: hwint_slave 13
HwInt14: hwint_slave 14
HwInt15: hwint_slave 15

;; @A+
;; @E+
DivideErrorExc:			exception_isr 0
SingleStepExc:			exception_isr 1
NmiExc:					exception_isr 2
;; @A-
BpExc:					exception_isr 3
OverflowExc:			exception_isr 4
BoundsCheckExc:			exception_isr 5
InvalidOpcodeExc:		exception_isr 6
CoCpuExc:				exception_isr 7
DoubleFaultExc:			exception_isr_error 8
CoCpuOverrunExc:		exception_isr 9
InvalidTssExc:			exception_isr_error 10
SegNotPresentExc:		exception_isr_error 11
StackExc:				exception_isr_error 12
GenProtectionExc:		exception_isr_error 13
PageFaultExc:			exception_isr_error 14
CoCpuErrorExc:			exception_isr 15
x87FpuFloatingPointExc:	exception_isr 16
AlignmentCheckExc:		exception_isr_error 17
MachineCheckExc:		exception_isr 18
SimdFloatingPointExc:	exception_isr 19
;; @A+
VirtualisationExc:		exception_isr 20
; 21-31 are Intel reserved.


segment .code;
; keep this dummy section for now so that the kernel image contains the heap,
; otherwise the image is truncated. To deal with this properly, need a way to
; build image with heap size in header. The linker script writes a 32-bit value
; in this section.
;; @A-
;; @E-

; END OF FILE ;
