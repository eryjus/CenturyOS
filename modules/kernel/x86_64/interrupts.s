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
    extern      LapicGetId
    extern      LapicEoi
    extern      DbgPrintf

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
    global      int20
    global      intxx


STATUS          EQU                 24
PREV_STS        EQU                 28
CPU_EXCEPTION   EQU                 6
CPU_SERVICE     EQU                 7


    bits        64
    section     .text


;;
;; -- Macro to handle the setting the `gs` section register properly.  The parameter is:
;;    Is there an error code on the stack? (1 = yes, 0 = no)
;;    ----------------------------------------------------------------------------------
%macro INT_PROLOG 1
    test        qword [rsp+8+(%1*8)],3          ;; test if we came from something other than ring 0
    jz          .noSwap                         ;; if from ring 0, skip the next step

    swapgs                                      ;; set the kernel `gs` base offset

.noSwap:
%endmacro



;;
;; -- Macro to handle the setting up the interrupt/exception context.  The parameter is:
;;    What is the context for this handler? (i.e.: CPU_EXCEPTION)
;;    ----------------------------------------------------------------------------------
%macro SET_CONTEXT 1
    mov         rdi,[gs:8]                      ;; from the kernel data structure, get the cpu addr
    lea         rsi,[rdi+STATUS]                ;; load the address of the status field
    lea         rdi,[rdi+PREV_STS]              ;; load the address of the prev status field

    mov         rax,[rsi]                       ;; get the current context
    mov         [rdi],rax                       ;; save it for return

    mov         rax,%1                          ;; get the new context
    mov         [rsi],rax                       ;; and set it
%endmacro


%macro RESTORE_CONTEXT 0
    mov         rdi,[gs:8]                      ;; from the kernel data structure, get the cpu addr
    lea         rsi,[rdi+STATUS]                ;; load the address of the status field
    lea         rdi,[rdi+PREV_STS]              ;; load the address of the prev status field

    mov         rax,[rdi]                       ;; get the prior context
    mov         [rsi],rax                       ;; and set it
%endmacro


%macro INT_EPILOG 1
    test        qword [rsp+8+(%1*8)],3          ;; test if we came from something other than ring 0
    jz          .exit                           ;; if from ring 0, skip the next step

    swapgs                                      ;; set the kernel `gs` base offset

.exit:
    iretq
%endmacro


;;
;; -- This is a macro to mimic the pusha instruction which is not available in x64
;;    ----------------------------------------------------------------------------
%macro  PUSHA 0
        push    rax
        push    rbx
        push    rcx
        push    rdx
        push    rbp
        push    rsi
        push    rdi
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
%endmacro



;;
;; -- This is a macro to mimic the popa instrucion, which is not available in x64
;;    ---------------------------------------------------------------------------
%macro  POPA 0
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rdi
        pop     rsi
        pop     rbp
        pop     rdx
        pop     rcx
        pop     rbx
        pop     rax
%endmacro



;;
;; -- Handle a #DE Fault
;;    ------------------
int00:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt00
    call        KernelPanic


;;
;; -- Handle a #DB Fault/Trap
;;    -----------------------
int01:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt01
    call        KernelPanic


;;
;; -- Handle a #MNI Interrupt
;;    -----------------------
int02:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt02
    call        KernelPanic


;;
;; -- Handle a #BP Trap
;;    -----------------
int03:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt03
    call        KernelPanic


;;
;; -- Handle a #OF Trap
;;    -----------------
int04:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt04
    call        KernelPanic


;;
;; -- Handle a #BR Fault
;;    ------------------
int05:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt05
    call        KernelPanic


;;
;; -- Handle a #UD Fault
;;    ------------------
int06:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt06
    call        KernelPanic


;;
;; -- Handle a #NM Fault
;;    ------------------
int07:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt07
    call        KernelPanic


;;
;; -- Handle a #DF Abort
;;    ------------------
int08:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt08
    call        KernelPanic


;;
;; -- Legacy Coprocessor Segment Overrun
;;    ----------------------------------
int09:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt09
    call        KernelPanic


;;
;; -- Handle a #TS Fault
;;    ------------------
int0a:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt0a
    call        KernelPanic


;;
;; -- Handle a #NP Fault
;;    ------------------
int0b:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt0b
    call        KernelPanic


;;
;; -- Handle a #SS Fault
;;    ------------------
int0c:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt0c
    call        KernelPanic


;;
;; -- Handle a #GP Fault
;;    ------------------
int0d:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt0d
    call        KernelPanic


;;
;; -- Handle a #PF Fault
;;    ------------------
int0e:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt0e
    call        KernelPanic

;;
;; -- if this interrupt returns, will need to determine if we need to swap gs again (sample below)
;;    --------------------------------------------------------------------------------------------
;    RESTORE_CONTEXT
;    INT_EPILOG(1)


;;
;; -- Reserved Interrupt
;;    ------------------
int0f:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt0f
    call        KernelPanic


;;
;; -- Handle a #MF Fault
;;    ------------------
int10:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt10
    call        KernelPanic


;;
;; -- Handle an #AC Fault
;;    -------------------
int11:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt11
    call        KernelPanic


;;
;; -- Handle an #MC Abort
;;    -------------------
int12:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt12
    call        KernelPanic


;;
;; -- Handle an #XM Fault
;;    -------------------
int13:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt13
    call        KernelPanic


;;
;; -- Handle an #VE Fault
;;    -------------------
int14:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt14
    call        KernelPanic


;;
;; -- Handle an #CP Fault
;;    -------------------
int15:
    INT_PROLOG(1)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt15
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int16:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt16
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int17:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt17
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int18:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt18
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int19:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt19
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1a:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt1a
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1b:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt1b
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1c:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt1c
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1d:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt1d
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1e:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt1e
    call        KernelPanic


;;
;; -- Reserved Interrupt
;;    ------------------
int1f:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_EXCEPTION)

    mov         rdi,msgInt1f
    call        KernelPanic



;;
;; -- Timer IRQ entry point
;;    ---------------------
int20:
    INT_PROLOG(0)
    SET_CONTEXT(CPU_SERVICE)
    PUSHA

    call        LapicGetId
    mov         rsi,rax
    mov         rdi,tick
    mov         eax,1
    call        DbgPrintf
    call        LapicEoi

    POPA
    RESTORE_CONTEXT
    INT_EPILOG(0)




;;
;; -- IRQ or software-generated interrupt
;;    -----------------------------------
intxx:
    iretq



    section     .rodata

tick:
    db          '%d',0

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



