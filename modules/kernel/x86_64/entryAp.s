;;===================================================================================================================
;;
;;  entryAp.s -- Entry point for the additional Application Processors for x86_64
;;
;;        Copyright (c)  2022 -- Adam Clark
;;        Licensed under "THE BEER-WARE LICENSE"
;;        See License.md for details.
;;
;;  This source has very different goals than the `entry.s` source. It is not concerned the setting up the system
;;  for the first time.  It is, however, required to leverage that setup for the additional processors.  Also,
;;  different than the boot processor, these start in 16-bit real mode.  I need to get out of that as quick as I
;;  can.
;;
;; -----------------------------------------------------------------------------------------------------------------
;;
;;     Date      Tracker  Version  Pgmr  Description
;;  -----------  -------  -------  ----  --------------------------------------------------------------------------
;;  2022-Mar-07  Initial  v0.0.1   ADCL  Initial version
;;
;;===================================================================================================================



    global      entryAp
    global      apLock
    global      kStack
    global      eEntry

    extern      idtrFinal
    extern      gdtrFinal

    section     .smptext
    bits        16


TRAMP_OFF       equ         0x8000
PAGE_SIZE       equ         0x1000



;;
;; -- This is the entry point for the boot processors
;;    -----------------------------------------------
entryAp:
;; create some space for a small stack
    jmp         0:(overData - entryAp) + TRAMP_OFF

    align       8

apLock:
    dd          0

apPml4:
    dd          0

kStack:
    dq          0

kEntry:
    dq          0

gdt64:
    dq          0                               ;; GDT entry 0x00 (NULL)
    dq          0x00a09a0000000000              ;; GDT entry 0x08 (KERNEL CODE)
    dq          0x00a0920000000000              ;; GDT entry 0x10 (KERNEL DATA)
gdt64End:

gdtr64:                                         ;; this is the GDT to jump into long mode
    dw          gdt64End - gdt64 - 1
    dd          (gdt64 - entryAp) + TRAMP_OFF


overData:
    cli                                         ;; set up the environment
    cld

    mov         ax,cs                           ;; clean up the segment registers
    mov         ds,ax
    mov         es,ax
    mov         ss,ax
    mov         sp,TRAMP_OFF + PAGE_SIZE

;;
;; -- This is a safety net to keep only one core from moving on at a time.  There is a race to get here
;;    when more than 1 core is started at a time.  The spinlock will work as a boundary to keep the others
;;    at bay until it is safe to continue.
;;    ----------------------------------------------------------------------------------------------------
    mov         cx,1                            ;; this is the value to load
    mov         bx,(apLock - entryAp) + TRAMP_OFF;; This is the address of apLock -- an offset from the seg start!

loop:
    mov         ax,0                            ;; this is the value it should be to load
LOCK xchg       [bx],cx                         ;; do the xchg -- notice the LOCK prefix
    cmp         ax,ax
    jnz         loop                            ;; if the lock was not unlocked, loop

;;
;; -- only 1 core at a time gets here -- set up the stack segment register (real mode!)
;;    ---------------------------------------------------------------------------------
    mov         al,0xff                         ;; Out 0xff to 0xA1 and 0x21 to disable all IRQs.
    out         0xa1,al
    out         0x21,al

    nop
    nop

;; -- Set the control registers for long mode
SetupLong:
    mov         eax,10100000b                   ;; Set the PAE and PGE bit
    mov         cr4,eax

    mov         eax,[(apPml4 - entryAp) + TRAMP_OFF];; Set the pml4 paging tables
    mov         cr3,eax

    mov         ecx,0xc0000080                  ;; Read from the EFER MSR
    rdmsr

    or          eax,0x00000100                  ;; Set the LME bit
    wrmsr

    mov         ebx,cr0                         ;; Activate long mode -
    or          ebx,0x80000001                  ;; - by enabling paging and protection simultaneously
    mov         cr0,ebx

    lgdt        [(gdtr64 - entryAp) + TRAMP_OFF];; Load gdtr64 defined from the original boot

;;
;; -- using the stack, perform a far return to Long Mode!
;;    ---------------------------------------------------
    jmp         8:(LongMode - entryAp) + TRAMP_OFF


;;
;; == Anything after here must be 64-bit code!
;;    ========================================
    align       8
    bits        64


LongMode:
    mov         rax,0x10
    mov         ds,ax
    mov         es,ax
    mov         fs,ax
    mov         gs,ax
    mov         ss,ax
    mov         rsp,[(kStack - entryAp) + TRAMP_OFF]
    mov         rbx,rsp

    mov         rax,idtrFinal
    lidt        [rax]

    mov         rax,gdtrFinal
    lgdt        [rax]

    push        qword 0
    push        rbx
    pushfq
    push        qword 0x08
    mov         rax,NewGdt
    push        rax
    iretq

NewGdt:
    mov         rax,0x10
    mov         ss,ax

    mov         rax,0x28
    mov         ds,ax
    mov         es,ax

    mov         rax,0
    mov         fs,ax
    mov         gs,ax

    mov         dword [(apLock - entryAp) + TRAMP_OFF],0
    mov         rax,(kEntry - entryAp) + TRAMP_OFF
    jmp         [rax]


