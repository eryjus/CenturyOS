;;===================================================================================================================
;;
;;  @file               entry.s
;;  @brief              Kernel Entry Point
;;  @author             Adam Clark (hobbyos@eryjus.com)
;;  @date               2022-Feb-24
;;  @since              v0.0.0
;;
;;  @copyright          Copyright (c)  2022 -- Adam Clark\n
;;                      Licensed under "THE BEER-WARE LICENSE"\n
;;                      See \ref LICENSE.md for details.
;;
;;  This file contains the entry point for the kernel.  It's purpose is to initialize the system enough to get
;;  into Long Mode and Upper Memory.
;;
;;  When this entry gets control, we are already in 32-bit protected mode.  There will be a small stack and a
;;  temporary GDT installed.  To get properly set up, the following tasks need to be completed:
;;  * Install the permanent GDT
;;  * Establish Paging
;;  * Enable Long Mode and jump to upper memory
;;
;;  To get there, several frames will need to be allocated, which will need to be later communicated to the PMM.
;;
;; -----------------------------------------------------------------------------------------------------------------
;;
;;  |     Date    | Tracker |  Version | Pgmr | Description
;;  |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
;;  | 2022-Feb-24 | Initial |  v0.0.0  | ADCL | Initial version
;;
;;===================================================================================================================


    extern      kInit
    extern      SerialOpen
    extern      roStart
    extern      kernelEnd
    extern      _bssEnd
    extern      _bssStart


    global      entry
    global      earlyFrame
    global      gdtFinal
    global      idtFinal
    global      gdtrFinal
    global      idtrFinal
    global      pml4


    section     .mboot

;;
;; -- Some equates to make things easier
;;    ----------------------------------
LEN2            equ         mb2HdrEnd - mb2HdrBeg
MAGIC2          equ         0xe85250d6
CHECK2          equ         (-(MAGIC2 + LEN2) & 0xffffffff)
MODE_TYPE       equ         0
WIDTH           equ         1024
HEIGHT          equ         768
DEPTH           equ         16
STACK           equ         0xffffc00000000000
PML4_ENT        equ         (((STACK-0x4000)>>(12+9+9+9))&0x1ff)<<3
PDPT_ENT        equ         (((STACK-0x4000)>>(12+9+9+0))&0x1ff)<<3
PD_ENT          equ         (((STACK-0x4000)>>(12+9+0+0))&0x1ff)<<3
PT_ENT          equ         (((STACK-0x4000)>>(12+0+0+0))&0x1ff)<<3


;;
;; -- This is the multiboot 2 header
;;    ------------------------------
    align       8
mb2HdrBeg:
    dd          MAGIC2
    dd          0                               ;; architecture: 0=32-bit protected mode
    dd          LEN2                            ;; total length of the mb2 header
    dd          CHECK2                          ;; mb2 checksum

    align       8
Type1Start:
    dw          1                               ;; type=1
    dw          0                               ;; not optional
    dd          Type1End-Type1Start             ;; size = 40
    dd          1                               ;; provide boot command line
    dd          2                               ;; provide boot loader name
    dd          3                               ;; provide module info
    dd          4                               ;; provide basic memory info
    dd          5                               ;; provide boot device
    dd          6                               ;; provide memory map
    dd          8                               ;; provide frame buffer info
    dd          9                               ;; provide elf symbol table
Type1End:

    align       8
Type4Start:
    dw          4                               ;; type=4
    dw          0                               ;; not optional
    dd          Type4End-Type4Start             ;; size = 12
    dd          1                               ;; graphics
Type4End:

    align       8
Type5Start:
    dw          5                               ;; graphic mode
    dw          1                               ;; not optional
    dd          Type5End-Type5Start             ;; size = 20
    dd          WIDTH                           ;; 1024
    dd          HEIGHT                          ;; 768
    dd          DEPTH                           ;; 16
Type5End:

    align       8
Type6Start:
    dw          6                               ;; Type=6
    dw          1                               ;; Not optional
    dd          Type6End-Type6Start             ;; size = 8 bytes even tho the doc says 12
Type6End:

    align       8
Type8Start:
    dw          0                               ;; Type=0
    dw          0                               ;; flags=0
    dd          8                               ;; size=8
mb2HdrEnd:


;;
;; -- the next early frame to allocate
;;    --------------------------------
earlyFrame:
    dq          (4 * 1024)                      ;; we start allocating frames at 16M (will access by dword here)


;;
;; -- Multiboot information
;;    ---------------------
mbSig:
    dd          0
mbData:
    dd          0


;;
;; -- The top of the stack
;;    --------------------
tos:
    dq          0


;;
;; -- we will use these variables to keep track of the paging tables
;;    --------------------------------------------------------------
pml4:
    dq          0
pdpt:
    dq          0
pd:
    dq          0
pt:
    dq          0



;;
;; -- The following is our GDT we need to enter 64-bit code
;;    -----------------------------------------------------
    align       8

gdt64:
    dq          0                               ;; GDT entry 0x00 (NULL)
    dq          0x00a09a0000000000              ;; GDT entry 0x08 (KERNEL CODE)
    dq          0x00a0920000000000              ;; GDT entry 0x10 (KERNEL DATA)
gdt64End:

gdtr64:                                         ;; this is the GDT to jump into long mode
    dw          gdt64End-gdt64-1
    dd          gdt64

idtr64:                                         ;; this is the GDT to jump into long mode
    dw          (256*16)-1
    dd          0


;;
;; == This is the main entry point for the loader
;;    ===========================================
    align       4
    bits        32


entry:
;; -- Save the Multi-boot Signature and Data Structure pointer
    mov         [mbSig],eax
    mov         [mbData],ebx


;; -- Start be allocating a 16K stack
    add         dword [earlyFrame],4            ;; pre-adjust the stack to the top
    mov         eax,[earlyFrame]
    shl         eax,12
    mov         [tos],eax
    mov         esp,eax                         ;; set the stack


;; -- start by allocating the 4 paging tables (pml4, pdpt, pd, pt)
    call        NewFrame
    mov         [pml4],eax

    call        NewFrame
    mov         [pdpt],eax

    call        NewFrame
    mov         [pd],eax

    call        NewFrame
    mov         [pt],eax


;; -- we have the page table in eax; set the entry to be identity mapped (assume 1 page)
    mov         edx,0x100000                    ;; the starting point for mapping
    mov         ebp,0x100000
    call        GetFlags
    or          edx,ebx                         ;; merge the flags
    mov         ecx,0x800                       ;; Starting point to map pages

.loop1:
    mov         [eax+ecx],edx                   ;; map the page: present, rw, global flags
    add         edx,0x1000                      ;; next frame
    add         ebp,0x1000                      ;; next frame
    call        GetFlags
    or          edx,ebx
    add         ecx,8                           ;; next PTE
    cmp         ecx,0x1000                      ;; done?
    jne         .loop1

    or          eax,0x03                        ;; set the flags for later

    mov         esi,[pd]                        ;; get the page directory and populate
    mov         [esi],eax
    mov         eax,esi                         ;; get ready to move up a level
    or          eax,0x03                        ;; set the flags for later

    mov         esi,[pdpt]                      ;; get the pdpt and populate
    mov         [esi],eax
    mov         eax,esi                         ;; get ready to move up a level
    or          eax,0x03                        ;; set the flags for later

    mov         esi,[pml4]                      ;; get the pml4 and populate
    mov         [esi],eax                       ;; identity mapped
    mov         [esi+0xc00],eax                 ;; upper memory location
    mov         eax,esi
    or          eax,0x03                        ;; set up for recursive mapping
    mov         [esi+0xff8],eax                 ;; recursively map the tables


;;
;; -- Now, map the stack
;;    ------------------
    call        NewFrame
    mov         [pdpt],eax

    call        NewFrame
    mov         [pd],eax

    call        NewFrame
    mov         [pt],eax

    mov         edx,[tos]                       ;; map the future stack (4 pages)
    sub         edx,0x1000
    or          edx,0x03                        ;; we know we are mapping the stack; must be read/write

    mov         [eax+PT_ENT+24],edx
    sub         edx,0x1000

    mov         [eax+PT_ENT+16],edx
    sub         edx,0x1000

    mov         [eax+PT_ENT+8 ],edx
    sub         edx,0x1000

    mov         [eax+PT_ENT+0 ],edx

    or          eax,0x03                        ;; set the flags for later

    mov         esi,[pd]                        ;; get the page directory and populate
    mov         [esi+PD_ENT],eax
    mov         eax,esi                         ;; get ready to move up a level
    or          eax,0x03                        ;; set the flags for later

    mov         esi,[pdpt]                      ;; get the pdpt and populate
    mov         [esi+PDPT_ENT],eax
    mov         eax,esi                         ;; get ready to move up a level
    or          eax,0x03                        ;; set the flags for later

    mov         esi,[pml4]                      ;; get the pml4 and populate
    mov         [esi+PML4_ENT],eax
    mov         [esi+0xbf8],eax


;;
;; -- Now, create an IDT and initialize it
;;    ------------------------------------
    call        NewFrame
    mov         [idtr64+2],eax


;;
;; -- load the IDT & GDT; jump to Long Mode
;;    -------------------------------------
    lidt        [idtr64]
    lgdt        [gdtr64]


;;
;; -- now, prepare to enter long mode
;;    -------------------------------
    mov         eax,cr4                         ;; get cr4
    or          eax,0x000000a0                  ;; enable PAE and PGE
    mov         cr4,eax                         ;; write the control register back

    mov         eax,[pml4]                      ;; get the top level address
    mov         cr3,eax                         ;; write teh control register

    mov         ecx,0xC0000080                  ;; get the EFER
    rdmsr

    or          eax,0x00000100                  ;; set the LME bit
    wrmsr                                       ;; and write the register back

    mov         eax,cr0                         ;; get cr0
    or          eax,0x80000001                  ;; enable paging (and protection)
    mov         cr0,eax                         ;; write the control register back (paging is enabled!)


    jmp         08:LongMode                     ;; jump into long mode


;;
;; -- Allocate a new frame, ensuring it is cleared before anything is done
;;
;;    Clobbers:
;;    * ecx
;;    * edx
;;    * edi
;;    --------------------------------------------------------------------
NewFrame:
    mov         edx,[earlyFrame]
    inc         dword [earlyFrame]
    shl         edx,12                          ;; this is our return value

    mov         edi,edx
    xor         eax,eax                         ;; clear eax
    cld
    mov         ecx,1024
    rep         stosd

    mov         eax,edx                         ;; get our return value

    ret



;;
;; -- Get the page flags for a kernel address
;;
;;    parameters:
;;    * ebp -- the lower 16-bits of the address to map (the upper 16
;;             bits are assumed to be `0xffffc000`); this register is
;;             preserved
;;
;;    returns:
;;    * ebx -- the flags to apply to the page given the address
;;
;;    all other registers are preserved
;;    --------------------------------------------------------------------
GetFlags:
    push        eax
    push        edx

    mov         ebx,0x01

    mov         eax,[kernelEnd]
    cmp         ebp,eax
    jb         .exit

    or          ebx,0x02

.exit:
    pop         edx
    pop         eax
    ret




;;===================================================================================================================
;;===================================================================================================================
;;===================================================================================================================
;;===================================================================================================================



;;
;; == Everything after this point is 64-bit LONG MODE!
;;    ================================================


    bits        64
    align       8

LongMode:
    mov         rax,0x10
    mov         ds,ax
    mov         es,ax
    mov         fs,ax
    mov         gs,ax
    mov         ss,ax
    mov         rsp,STACK

    mov         rax,HigherHalf
    jmp         rax


;;===================================================================================================================
;;===================================================================================================================
;;===================================================================================================================
;;===================================================================================================================


    section     .data

    align       4096

idtFinal:
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dq          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
idtFinalEnd:

gdtFinal:
    dq          0                               ;; GDT entry 0x00 (NULL)
    dq          0x00a09a0000000000              ;; GDT entry 0x08 (KERNEL CODE)
    dq          0x00a0920000000000              ;; GDT entry 0x10 (KERNEL STACK)
    dq          0x00cffa000000ffff              ;; GDT entry 0x18 (USER CODE)
    dq          0x00cff2000000ffff              ;; GDT entry 0x20 (USER STACK)
    dq          0x00a0920000000000              ;; GDT entry 0x28 (KERNEL DATA)
    dq          0x00cff2000000ffff              ;; GDT entry 0x30 (USER DATA)
    dq          0                               ;; GDT entry 0x38 (Future Use)
    dq          0                               ;; GDT entry 0x40 (Future Use)
    dq          0                               ;; GDT entry 0x48 (Future Use)
    dq          0                               ;; GDT entry 0x50 (Future Use)
    dq          0                               ;; GDT entry 0x58 (Future Use)
    dq          0                               ;; GDT entry 0x60 (Future Use)
    dq          0                               ;; GDT entry 0x68 (Future Use)
    dq          0                               ;; GDT entry 0x70 (Future Use)
    dq          0                               ;; GDT entry 0x78 (Future Use)
    dq          0                               ;; GDT entry 0x80 (Future Use)
    dq          0                               ;; GDT entry 0x88 (Future Use)
    dq          0                               ;; GDT entry 0x90 (Future Use)

    ;; -- CPU0
    dq          0x00a0920000000000              ;; GDT entry 0x98 (gs: for CPU0)
    dq          0                               ;; GDT entry 0xa0 (reserved: Upper TSS for CPU0)
    dq          0                               ;; GDT entry 0xa8 (reserved: Lower TSS for CPU0)

    ;; -- CPU1
    dq          0x00a0920000000000              ;; GDT entry 0xb0 (gs: for CPU1)
    dq          0                               ;; GDT entry 0xb8 (reserved: Upper TSS for CPU1)
    dq          0                               ;; GDT entry 0xc0 (reserved: Lower TSS for CPU1)

    ;; -- CPU2
    dq          0x00a0920000000000              ;; GDT entry 0xc8 (gs: for CPU2)
    dq          0                               ;; GDT entry 0xd0 (reserved: Upper TSS for CPU2)
    dq          0                               ;; GDT entry 0xd8 (reserved: Lower TSS for CPU2)

    ;; -- CPU3
    dq          0x00a0920000000000              ;; GDT entry 0xe0 (gs: for CPU3)
    dq          0                               ;; GDT entry 0xe8 (reserved: Upper TSS for CPU3)
    dq          0                               ;; GDT entry 0xf0 (reserved: Lower TSS for CPU3)
gdtFinalEnd:

gdtrFinal:                                      ;; this is the final GDT
    dw          gdtFinalEnd-gdtFinal-1
    dq          gdtFinal

idtrFinal:                                      ;; this is the final IDT
    dw          idtFinalEnd-idtFinal-1
    dq          idtFinal




;;
;; == Everything after this point resides in upper memory
;;    ===================================================


    section     .text

HigherHalf:
    mov         rax,gdtrFinal
    lgdt        [rax]

    mov         rax,idtrFinal
    lidt        [rax]


;;
;; -- need to perform an iret to trigger the new final GDT
;;    ----------------------------------------------------
    mov         rax,0x10
    push        rax
    mov         rax,STACK
    push        rax
    pushfq
    mov         rax,0x08
    push        rax
    mov         rax,.realGdt
    push        rax

    iretq


;;
;; -- refresh segment selectors
;;    -------------------------
.realGdt:
    mov         rax,0x10
    mov         ss,ax

    mov         rax,0x28
    mov         ds,ax
    mov         es,ax

    mov         rax,0x0
    mov         fs,ax
    mov         gs,ax

    mov         rcx,_bssEnd
    mov         rdi,_bssStart
    xor         rax,rax
    cld

    sub         rcx,rdi
    shr         rcx,3

    rep         stosq

    call        SerialOpen

    jmp         kInit

