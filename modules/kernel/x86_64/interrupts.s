;;===================================================================================================================
;;
;;  @file               interrupts.s
;;  @brief              Internal implementation of Messages and Message Queues
;;  @author             Adam Clark (hobbyos@eryjus.com)
;;  @date               2022-Feb-26
;;  @since              v0.0.0
;;
;;  @copyright          Copyright (c)  2022 -- Adam Clark\n
;;                      Licensed under "THE BEER-WARE LICENSE"\n
;;                      See \ref LICENSE.md for details.
;;
;;  This file contains the interrupt/exception entry points.
;;
;; -----------------------------------------------------------------------------------------------------------------
;;
;;  |     Date    | Tracker |  Version | Pgmr | Description
;;  |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
;;  | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
;;
;;===================================================================================================================


    extern      KernelPanic

    global      int00
    global      int01
    global      int02
    global      int03
    global      int04
    global      int05
    global      int06
    global      int07
    global      int08
    global      int09
    global      int0a
    global      int0b
    global      int0c
    global      int0d
    global      int0e
    global      int0f
    global      int10
    global      int11
    global      int12
    global      int13
    global      int14
    global      int15
    global      int16
    global      int17
    global      int18
    global      int19
    global      int1a
    global      int1b
    global      int1c
    global      int1d
    global      int1e
    global      int1f
    global      intxx


    bits        64
    section     .text


;;
;; -- Handle a #DE Fault
;;    ------------------
int00:
    mov         rdi,msgInt00
    call        KernelPanic


;;
;; -- Handle a #DB Fault/Trap
;;    -----------------------
int01:
    mov         rdi,msgInt01
    call        KernelPanic


;;
;; -- Handle a #MNI Interrupt
;;    -----------------------
int02:
    mov         rdi,msgInt02
    call        KernelPanic


;;
;; -- Handle a #BP Trap
;;    -----------------
int03:
    mov         rdi,msgInt03
    call        KernelPanic


;;
;; -- Handle a #OF Trap
;;    -----------------
int04:
    mov         rdi,msgInt04
    call        KernelPanic


;;
;; -- Handle a #BR Fault
;;    ------------------
int05:
    mov         rdi,msgInt05
    call        KernelPanic


;;
;; -- Handle a #UD Fault
;;    ------------------
int06:
    mov         rdi,msgInt06
    call        KernelPanic


;;
;; -- Handle a #NM Fault
;;    ------------------
int07:
    mov         rdi,msgInt07
    call        KernelPanic


;;
;; -- Handle a #DF Abort
;;    ------------------
int08:
    mov         rdi,msgInt08
    call        KernelPanic


;;
;; -- Legacy Coprocessor Segment Overrun
;;    ----------------------------------
int09:
    mov         rdi,msgInt09
    call        KernelPanic


;;
;; -- Handle a #TS Fault
;;    ------------------
int0a:
    mov         rdi,msgInt0a
    call        KernelPanic


;;
;; -- Handle a #NP Fault
;;    ------------------
int0b:
    mov         rdi,msgInt0b
    call        KernelPanic


;;
;; -- Handle a #SS Fault
;;    ------------------
int0c:
    mov         rdi,msgInt0c
    call        KernelPanic


;;
;; -- Handle a #GP Fault
;;    ------------------
int0d:
    mov         rdi,msgInt0d
    call        KernelPanic


;;
;; -- Handle a #PF Fault
;;    ------------------
int0e:
    mov         rdi,msgInt0e
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int0f:
    mov         rdi,msgInt0f
    call        KernelPanic


;;
;; -- Handle a #MF Fault
;;    ------------------
int10:
    mov         rdi,msgInt10
    call        KernelPanic


;;
;; -- Handle an #AC Fault
;;    -------------------
int11:
    mov         rdi,msgInt11
    call        KernelPanic


;;
;; -- Handle an #MC Abort
;;    -------------------
int12:
    mov         rdi,msgInt12
    call        KernelPanic


;;
;; -- Handle an #XM Fault
;;    -------------------
int13:
    mov         rdi,msgInt13
    call        KernelPanic


;;
;; -- Handle an #VE Fault
;;    -------------------
int14:
    mov         rdi,msgInt14
    call        KernelPanic


;;
;; -- Handle an #CP Fault
;;    -------------------
int15:
    mov         rdi,msgInt15
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int16:
    mov         rdi,msgInt16
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int17:
    mov         rdi,msgInt17
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int18:
    mov         rdi,msgInt18
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int19:
    mov         rdi,msgInt19
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1a:
    mov         rdi,msgInt1a
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1b:
    mov         rdi,msgInt1b
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1c:
    mov         rdi,msgInt1c
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1d:
    mov         rdi,msgInt1d
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1e:
    mov         rdi,msgInt1e
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1f:
    mov         rdi,msgInt1f
    call        KernelPanic


;;
;; -- IRQ or software-generated interrupt
;;    -----------------------------------
intxx:
    iretq


    section     .rodata

msgInt00:
    db          '#DE -- Divide Error',0

msgInt01:
    db          '#DB -- Debug Exception',0

msgInt02:
    db          '#MNI -- Non-Maskable Interrupt',0

msgInt03:
    db          '#BP -- Breakpoint',0

msgInt04:
    db          '#OF -- Overflow',0

msgInt05:
    db          '#BR -- BOUND Range Exceeded',0

msgInt06:
    db          '#UD -- Invalid Opcode',0

msgInt07:
    db          '#NM -- Device Not Available',0

msgInt08:
    db          '#DF -- Double Fault',0

msgInt09:
    db          'Coprocessor Segment Overrun',0

msgInt0a:
    db          '#TS -- Invalid TSS',0

msgInt0b:
    db          '#NP -- Segment Not Present',0

msgInt0c:
    db          '#SS -- Stack-Segment Fault',0

msgInt0d:
    db          '#GP -- General Protection',0

msgInt0e:
    db          '#PF -- Page Fault',0

msgInt0f:
    db          '0x0f -- Reserved Interrupt',0

msgInt10:
    db          '#MF -- Math Fault',0

msgInt11:
    db          '#AC -- Alignment Check',0

msgInt12:
    db          '#MC -- Machine Check',0

msgInt13:
    db          '#XM -- SIMD Floating-Point Exception',0

msgInt14:
    db          '#VE -- Virtualization Exception',0

msgInt15:
    db          '#CP -- Control Protection Exception',0

msgInt16:
    db          '0x16 -- Reserved Interrupt',0

msgInt17:
    db          '0x17 -- Reserved Interrupt',0

msgInt18:
    db          '0x18 -- Reserved Interrupt',0

msgInt19:
    db          '0x19 -- Reserved Interrupt',0

msgInt1a:
    db          '0x1a -- Reserved Interrupt',0

msgInt1b:
    db          '0x1b -- Reserved Interrupt',0

msgInt1c:
    db          '0x1c -- Reserved Interrupt',0

msgInt1d:
    db          '0x1d -- Reserved Interrupt',0

msgInt1e:
    db          '0x1e -- Reserved Interrupt',0

msgInt1f:
    db          '0x1f -- Reserved Interrupt',0



