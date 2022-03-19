/****************************************************************************************************************//**
*   @file               arch.h
*   @brief              Types, constants, and inlines for the x86_64 arch
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#ifndef __ARCH_H__
#define __ARCH_H__



/****************************************************************************************************************//**
*   @def                EXTERNC
*   @brief              Inform the compiler not to mangle the name of this function
*///----------------------------------------------------------------------------------------------------------------
#define EXTERNC         extern "C"



/****************************************************************************************************************//**
*   @def                NORETURN
*   @brief              Inform the compiler that the function does not return and can be optimized as such
*///----------------------------------------------------------------------------------------------------------------
#if defined(__clang__)
# define NORETURN       _Noreturn
#else
# define NORETURN        __attribute__((NORETURN))
#endif



/****************************************************************************************************************//**
*   @def                INLINE
*   @brief              Inform the compiler to always inline this function
*///----------------------------------------------------------------------------------------------------------------
#define INLINE          inline __attribute__((always_inline))



/****************************************************************************************************************//**
*   @def                PACKED
*   @brief              Inform the compiler to pack the data structure rather than aligning each element
*///----------------------------------------------------------------------------------------------------------------
#define PACKED          __attribute__((packed))



/****************************************************************************************************************//**
*   @def                PACKED_16
*   @brief              Inform the compiler to pack align the data structure to the given address size
*///----------------------------------------------------------------------------------------------------------------
#define PACKED_16       __attribute__((packed, aligned(16)))



/****************************************************************************************************************//**
*   @def                KERNEL_CODE
*   @brief              Inform the compiler to place this function in the .text section for the kernel
*///----------------------------------------------------------------------------------------------------------------
#define KERNEL_CODE     __attribute__((section(".text")))



/****************************************************************************************************************//**
*   @def                KERNEL_RODATA
*   @brief              Inform the compiler to place this variable in the .rodata section for the kernel
*///----------------------------------------------------------------------------------------------------------------
#define KERNEL_RODATA   __attribute__((section(".rodata")))



/****************************************************************************************************************//**
*   @def                KERNEL_DATA
*   @brief              Inform the compiler to place this variable in the .data section for the kernel
*///----------------------------------------------------------------------------------------------------------------
#define KERNEL_DATA     __attribute__((section(".data")))



/****************************************************************************************************************//**
*   @def                KERNEL_BSS
*   @brief              Inform the compiler to place this variable in the .bss section for the kernel
*///----------------------------------------------------------------------------------------------------------------
#define KERNEL_BSS      __attribute__((section(".bss")))



/****************************************************************************************************************//**
*   @def                KRN_FUNC
*   @brief              This is a function to be placed in the kernel section
*///----------------------------------------------------------------------------------------------------------------
#define KRN_FUNC        EXTERNC KERNEL_CODE



/****************************************************************************************************************//**
*   @def                LZONE_CODE
*   @brief              Inform the compiler to place this function in the .text.lzone section for the kernel
*
*   @todo               This needs to be fixed up later and made its own section
*///----------------------------------------------------------------------------------------------------------------
#define LZONE_CODE      __attribute__((section(".text")))



/****************************************************************************************************************//**
*   @def                LZONE_RODATA
*   @brief              Inform the compiler to place this variable in the .rodata.lzone section for the kernel
*
*   @todo               This needs to be fixed up later and made its own section
*///----------------------------------------------------------------------------------------------------------------
#define LZONE_RODATA    __attribute__((section(".rodata")))



/****************************************************************************************************************//**
*   @def                LZONE_DATA
*   @brief              Inform the compiler to place this variable in the .data.lzone section for the kernel
*///----------------------------------------------------------------------------------------------------------------
#define LZONE_DATA      __attribute__((section(".data")))



/****************************************************************************************************************//**
*   @def                LZONE_BSS
*   @brief              Inform the compiler to place this variable in the .bss.lzone section for the kernel
*///----------------------------------------------------------------------------------------------------------------
#define LZONE_BSS       __attribute__((section(".bss")))



/****************************************************************************************************************//**
*   @def                LZONE_FUNC
*   @brief              This is a function to be placed in the landing zone section
*///----------------------------------------------------------------------------------------------------------------
#define LZONE_FUNC        EXTERNC LZONE_CODE







/****************************************************************************************************************//**
*   @def                MAX_CPU
*   @brief              The maximum number of CPUs supported on this architecture
*///----------------------------------------------------------------------------------------------------------------
#define MAX_CPU 4



/****************************************************************************************************************//**
*   @def                PAGE_SIZE
*   @brief              The size of a frame on this arch
*///-----------------------------------------------------------------------------------------------------------------
#define PAGE_SIZE 4096



/****************************************************************************************************************//**
*   @def                TRAMP_OFF
*   @brief              The location of the trampoline startup code
*///----------------------------------------------------------------------------------------------------------------
#define TRAMP_OFF 0x8000



/********************************************************************************************************************
*   Some flags used for mapping pages in the kernel
*///-----------------------------------------------------------------------------------------------------------------
enum {
  PG_WRT = 0x00000001, //!< VMM page is writable
  PG_DEV = 0x00000002, //!< VMM page is not cacheable and it supervisor
  PG_KRN = 0x00000004, //!< VMM Page is supervisor and is not swapable (but may
                       //!< be not present)
};



/****************************************************************************************************************//**
*   @typedef            size_t
*   @brief              An unsigned size of not less that 16 bit width
*///-----------------------------------------------------------------------------------------------------------------
typedef unsigned int size_t;
static_assert(sizeof(size_t) >= 2, "`size_t` must be 16 bits wide or more");



/****************************************************************************************************************//**
*   @typedef            uint64_t
*   @brief              An 8-byte (qword) unsigned integer
*///-----------------------------------------------------------------------------------------------------------------
typedef unsigned long uint64_t;
static_assert(sizeof(uint64_t) == 8, "`uint64_t` is not 8 bytes");



/****************************************************************************************************************//**
*   @typedef            uint32_t
*   @brief              A 4-byte (dword) unsigned integer
*///-----------------------------------------------------------------------------------------------------------------
typedef unsigned int uint32_t;
static_assert(sizeof(uint32_t) == 4, "`uint32_t` is not 4 bytes");



/****************************************************************************************************************//**
*   @typedef            uint16_t
*   @brief              A 2-byte (word) unsigned integer
*///-----------------------------------------------------------------------------------------------------------------
typedef unsigned short uint16_t;
static_assert(sizeof(uint16_t) == 2, "`uint16_t` is not 2 bytes");



/****************************************************************************************************************//**
*   @typedef            uint8_t
*   @brief              A 1-byte (byte) unsigned integer
*///-----------------------------------------------------------------------------------------------------------------
typedef unsigned char uint8_t;
static_assert(sizeof(uint8_t) == 1, "`uint8_t` is not 1 byte");



/****************************************************************************************************************//**
*   @typedef            int64_t
*   @brief              An 8-byte (qword) signed integer
*///-----------------------------------------------------------------------------------------------------------------
typedef long int64_t;
static_assert(sizeof(int64_t) == 8, "`int64_t` is not 8 bytes");



/****************************************************************************************************************//**
*   @typedef            int32_t
*   @brief              A 4-byte (dword) signed integer
*///-----------------------------------------------------------------------------------------------------------------
typedef int int32_t;
static_assert(sizeof(int32_t) == 4, "`int32_t` is not 4 bytes");



/****************************************************************************************************************//**
*   @typedef            int16_t
*   @brief              A 2-byte (word) signed integer
*///-----------------------------------------------------------------------------------------------------------------
typedef short int16_t;
static_assert(sizeof(int16_t) == 2, "`int16_t` is not 2 bytes");



/****************************************************************************************************************//**
*   @typedef            int8_t
*   @brief              A 1-byte (byte) signed integer
*///-----------------------------------------------------------------------------------------------------------------
typedef char int8_t;
static_assert(sizeof(int8_t) == 1, "`int8_t` is not 1 byte");



/****************************************************************************************************************//**
*   @typedef            Addr_t
*   @brief              The size of an address in this arch
*///-----------------------------------------------------------------------------------------------------------------
typedef uint64_t Addr_t;



/****************************************************************************************************************//**
*   @typedef            Frame_t
*   @brief              The size of a physical frame number
*///-----------------------------------------------------------------------------------------------------------------
typedef uint64_t Frame_t;



/****************************************************************************************************************//**
*   @struct             IdtDescriptor_t
*   @brief              The format of an interrupt descriptor table entry in the IDT
*
*   @note               See the Intel SDM Volume 3 Chapter 6.14.1 for more information
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @typedef            IdtDescriptor_t
*   @brief              This typedef formalizes the \ref IdtDescriptor_t structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct IdtDescriptor_t {
  unsigned int offsetLo : 16;       //!< Bits 0-15 of the target handler address
  unsigned int segmentSel : 16;     //!< The `cs` segment selector of the handler
  unsigned int ist : 3;             //!< The Index of the Stack to use for this handler (0=Legacy mode)
  unsigned int zero : 5;            //!< These bits are set to 0
  unsigned int type : 4;            //!< The handler type (0xe for Interrupt; 0xf for Trap)
  unsigned int off : 1;             //!< This bit is set to 0
  unsigned int dpl : 2;             //!< Descriptor Privilege Level
  unsigned int p : 1;               //!< Present
  unsigned int offsetMid : 16;      //!< Bits 16-31 of the target handler address
  unsigned int offsetHi : 32;       //!< Bits 32-63 of the target handler address
  unsigned int unused : 32;         //!< Unused; set to 0
} PACKED IdtDescriptor_t;



/****************************************************************************************************************//**
*   @typedef            PageEntry_t
*   @brief              Formalization of the \ref PageEntry_t structure into a defined type
*///----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             PageEntry_t
*   @brief              The structure of the x86_64-pc page table Entry
*
*   This structure is dictated by the x86_64 CPU architecture.  It has several bit fields which are directly
*   analogous to the bit fields in the MMU itself, so there is a direect translaction from C to the hardware.
*   This is a 64-bit Entry and this 1 structure represents all levels of the page tables.
*///----------------------------------------------------------------------------------------------------------------
typedef struct PageEntry_t {
  unsigned int p : 1;               //!< Is the page present?
  unsigned int rw : 1;              //!< set to 1 to allow writes
  unsigned int us : 1;              //!< 0=Supervisor; 1=user
  unsigned int pwt : 1;             //!< Page Write Through
  unsigned int pcd : 1;             //!< Page-level cache disable
  unsigned int a : 1;               //!< accessed
  unsigned int d : 1;               //!< dirty (needs to be written for a swap)
  unsigned int pat : 1;             //!< set to 0 for tables, page Page Attribute Table (set to 0)
  unsigned int g : 1;               //!< Global (set to 0)
  unsigned int k : 1;               //!< Is this a kernel-space (protected) page?
  unsigned int avl : 2;             //!< Available for software use
  Frame_t frame : 36;               //!< This is the 4K aligned page frame address (or table address)
  unsigned int reserved : 4;        //!< reserved bits
  unsigned int software : 11;       //!< software use bits
  unsigned int xd : 1;              //!< execute disable
} PACKED PageEntry_t;



/****************************************************************************************************************//**
*   @typedef            Tss_t
*   @brief              Formalization of the \ref Tss_t structure into a defined type
*///----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             Tss_t
*   @brief              64-bit Task State Segment
*
*   This structure is dictated by the x86_64 CPU architecture.  At least 1 TSS is required for each CPU.
*///----------------------------------------------------------------------------------------------------------------
typedef struct Tss_t {
  uint32_t reserved0;               //!< Unused
  uint32_t lowerRsp0;               //!< Lower Stack Pointer to use when interrupting from ring 0
  uint32_t upperRsp0;               //!< Upper Stack Pointer to use when interrupting from ring 0
  uint32_t lowerRsp1;               //!< Lower Stack Pointer to use when interrupting from ring 1
  uint32_t upperRsp1;               //!< Upper Stack Pointer to use when interrupting from ring 1
  uint32_t lowerRsp2;               //!< Lower Stack Pointer to use when interrupting from ring 2
  uint32_t upperRsp2;               //!< Upper Stack Pointer to use when interrupting from ring 2
  uint32_t lowerRsp3;               //!< Lower Stack Pointer to use when interrupting from ring 3
  uint32_t upperRsp3;               //!< Upper Stack Pointer to use when interrupting from ring 2
  uint32_t reserved1;               //!< Unused
  uint32_t reserved2;               //!< Unused
  uint32_t lowerIst1;               //!< Lower Stack Pointer to use when IST1 is specified
  uint32_t upperIst1;               //!< Upper Stack Pointer to use when IST1 is specified
  uint32_t lowerIst2;               //!< Lower Stack Pointer to use when IST2 is specified
  uint32_t upperIst2;               //!< Upper Stack Pointer to use when IST2 is specified
  uint32_t lowerIst3;               //!< Lower Stack Pointer to use when IST3 is specified
  uint32_t upperIst3;               //!< Upper Stack Pointer to use when IST3 is specified
  uint32_t lowerIst4;               //!< Lower Stack Pointer to use when IST4 is specified
  uint32_t upperIst4;               //!< Upper Stack Pointer to use when IST4 is specified
  uint32_t lowerIst5;               //!< Lower Stack Pointer to use when IST5 is specified
  uint32_t upperIst5;               //!< Upper Stack Pointer to use when IST5 is specified
  uint32_t lowerIst6;               //!< Lower Stack Pointer to use when IST6 is specified
  uint32_t upperIst6;               //!< Upper Stack Pointer to use when IST6 is specified
  uint32_t lowerIst7;               //!< Lower Stack Pointer to use when IST7 is specified
  uint32_t upperIst7;               //!< Upper Stack Pointer to use when IST7 is specified
  uint32_t reserved3;               //!< Unused
  uint32_t reserved4;               //!< Unused
  uint16_t reserved5;               //!< Unused
  uint16_t ioMapBase;               //!< Base offset to the start of the I/O permissions map
} PACKED Tss_t;



/****************************************************************************************************************//**
*   @typedef            ArchCpu_t
*   @brief              Formalization of the \ref ArchCpu_t structure into a defined type
*///----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             ArchCpu_t
*   @brief              x86_64-specific data elements required for the CPU abstraction
*
*   This structure is dictated by the abstractions required by the x86_64 CPU architecture.
*///----------------------------------------------------------------------------------------------------------------
typedef struct ArchCpu_t {
  Tss_t tss;                        //!< This is the x86_64 TSS
} ArchCpu_t;



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPml4Entry(Addr_t a)
*   @brief              Get the address of the PML4 Table Entry
*
*   @param              a                   The address for which to extract the PML4 Entry
*
*   @returns            A pointer to the PML4 Entry
*///-----------------------------------------------------------------------------------------------------------------
INLINE
PageEntry_t *GetPml4Entry(Addr_t a) {
    return &((PageEntry_t *)0xfffffffffffff000)[(a >> 39) & 0x1ff];
}



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPdptEntry(Addr_t a)
*   @brief              Get the address of the PDPT Table Entry
*
*   @param              a                   The address for which to extract the PDPT Entry
*
*   @returns            A pointer to the PDPT Entry
*///-----------------------------------------------------------------------------------------------------------------
INLINE
PageEntry_t *GetPdptEntry(Addr_t a) {
    return &((PageEntry_t *)0xffffffffffe00000)[(a >> 30) & 0x3ffff];
}



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPdEntry(Addr_t a)
*   @brief              Get the address of the PD Table Entry
*
*   @param              a                   The address for which to extract the PD Entry
*
*   @returns            A pointer to the PD Entry
*///-----------------------------------------------------------------------------------------------------------------
INLINE
PageEntry_t *GetPdEntry(Addr_t a) {
    return &((PageEntry_t *)0xffffffffc0000000)[(a >> 21) & 0x7ffffff];
}



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPtEntry(Addr_t a)
*   @brief              Get the address of the PT Table Entry
*
*   @param              a                   The address for which to extract the PT Entry
*
*   @returns            A pointer to the PT Entry
*///-----------------------------------------------------------------------------------------------------------------
INLINE
PageEntry_t *GetPtEntry(Addr_t a) {
    return &((PageEntry_t *)0xffffff8000000000)[(a >> 12) & 0xfffffffff];
}



/****************************************************************************************************************//**
*   @fn                 void ArchMmuMapPage(Addr_t a, Frame_t f, int flags)
*   @brief              Map a page in Virtual Memory Space to a Physical Frame
*
*   @param              a                   The address for which to map
*   @param              f                   The frame to which to map the page
*   @param              flags               Flags which will help control the mapping security
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ArchMmuMapPage(Addr_t a, Frame_t f, int flags);



/****************************************************************************************************************//**
*   @fn                 uint8_t INB(uint16_t port)
*   @brief              Get a byte from an I/O Port
*
*   @param              port                The I/O port to read
*
*   @returns            The byte read from the I/O port
*///-----------------------------------------------------------------------------------------------------------------
INLINE
uint8_t INB(uint16_t port) {
    uint8_t rv;
    __asm volatile("inb %1, %0" : "=a"(rv) : "Nd"(port));
    return rv;
}



/****************************************************************************************************************//**
*   @fn                 void INVLPG(Addr_t a)
*   @brief              Invalidate a page from the TLB
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void INVLPG(Addr_t a) {
    __asm volatile("invlpg (%0)" ::"r"(a) : "memory");
}



/****************************************************************************************************************//**
*   @fn                 void LTR(uint16_t tr)
*   @brief              Load the task register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void LTR(uint16_t tr) {
    __asm volatile("ltr  %0" ::"r"(tr) : "memory");
}



/****************************************************************************************************************//**
*   @fn                 void NOP(void)
*   @brief              Delay a short period of time
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void NOP(void) { __asm volatile("nop" ::: "memory"); }



/****************************************************************************************************************//**
*   @fn                 void OUTB(uint16_t port, uint8_t val)
*   @brief              Write a byte to an I/O Port
*
*   @param              port                The I/O port to which to write
*   @param              val                 The value to write to the I/O port
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void OUTB(uint16_t port, uint8_t val) {
    __asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
}



/****************************************************************************************************************//**
*   @fn                 void EnableInterrupts(void)
*   @brief              Enable Interrupts explicitly
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void EnableInterrupts(void) {
    __asm volatile("sti" ::: "memory");
}



/****************************************************************************************************************//**
*   @fn                 void SWAPGS(void)
*   @brief              Swap the `gs` register with the IA32_KERNEL_GS_BASE model-specific register (setting limits)
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void SWAPGS(void) {
    __asm volatile("swapgs" ::: "memory");
}



/****************************************************************************************************************//**
*   @fn                 void WBNOINVD(void)
*   @brief              Synchronize the cpu caches
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void WBNOINVD(void) {
    __asm volatile("wbnoinvd" ::: "memory");
}



/****************************************************************************************************************//**
*   @fn                 void int00(void)
*   @brief              Handle the \#DE Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int00(void);



/****************************************************************************************************************//**
*   @fn                 void int01(void)
*   @brief              Handle a \#DB Fault/Trap
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int01(void);



/****************************************************************************************************************//**
*   @fn                 void int02(void)
*   @brief              Handle a \#MNI Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int02(void);



/****************************************************************************************************************//**
*   @fn                 void int03(void)
*   @brief              Handle a \#BP Trap
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int03(void);



/****************************************************************************************************************//**
*   @fn                 void int04(void)
*   @brief              Handle a \#OF Trap
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int04(void);



/****************************************************************************************************************//**
*   @fn                 void int05(void)
*   @brief              Handle a \#BR Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int05(void);



/****************************************************************************************************************//**
*   @fn                 void int06(void)
*   @brief              Handle a \#UD Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int06(void);



/****************************************************************************************************************//**
*   @fn                 void int07(void)
*   @brief              Handle a \#NM Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int07(void);



/****************************************************************************************************************//**
*   @fn                 void int08(void)
*   @brief              Handle a \#DF Abort
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int08(void);



/****************************************************************************************************************//**
*   @fn                 void int09(void)
*   @brief              Legacy Coprocessor Segment Overrun
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int09(void);



/****************************************************************************************************************//**
*   @fn                 void int0a(void)
*   @brief              Handle a \#TS Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int0a(void);



/****************************************************************************************************************//**
*   @fn                 void int0b(void)
*   @brief              Handle a \#NP Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int0b(void);



/****************************************************************************************************************//**
*   @fn                 void int0c(void)
*   @brief              Handle a \#SS Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int0c(void);



/****************************************************************************************************************//**
*   @fn                 void int0d(void)
*   @brief              Handle a \#GP Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int0d(void);



/****************************************************************************************************************//**
*   @fn                 void int0e(void)
*   @brief              Handle a \#PF Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int0e(void);



/****************************************************************************************************************//**
*   @fn                 void int0f(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int0f(void);



/****************************************************************************************************************//**
*   @fn                 void int10(void)
*   @brief              Handle a \#MF Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int10(void);



/****************************************************************************************************************//**
*   @fn                 void int11(void)
*   @brief              Handle an \#AC Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int11(void);



/****************************************************************************************************************//**
*   @fn                 void int12(void)
*   @brief              Handle an \#MC Abort
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int12(void);



/****************************************************************************************************************//**
*   @fn                 void int13(void)
*   @brief              Handle an \#XM Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int13(void);



/****************************************************************************************************************//**
*   @fn                 void int14(void)
*   @brief              Handle an \#VE Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int14(void);



/****************************************************************************************************************//**
*   @fn                 void int15(void)
*   @brief              Handle an \#CP Fault
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int15(void);



/****************************************************************************************************************//**
*   @fn                 void int16(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int16(void);



/****************************************************************************************************************//**
*   @fn                 void int17(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int17(void);



/****************************************************************************************************************//**
*   @fn                 void int18(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int18(void);



/****************************************************************************************************************//**
*   @fn                 void int19(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int19(void);



/****************************************************************************************************************//**
*   @fn                 void int1a(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int1a(void);



/****************************************************************************************************************//**
*   @fn                 void int1b(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int1b(void);



/****************************************************************************************************************//**
*   @fn                 void int1c(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int1c(void);



/****************************************************************************************************************//**
*   @fn                 void int1d(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int1d(void);



/****************************************************************************************************************//**
*   @fn                 void int1e(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int1e(void);



/****************************************************************************************************************//**
*   @fn                 void int1f(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC NORETURN
void int1f(void);



/****************************************************************************************************************//**
*   @fn                 void int20(void)
*   @brief              Timer IRQ Interrupt
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC
void int20(void);



/****************************************************************************************************************//**
*   @fn                 void intxx(void)
*   @brief              Generically handle any other IRQ or software generated interrupt
*
*   @note               This handler will be used as a stopgap for all other interrupts.  Specific handlers will be
*                       installed to handle specific things.
*///-----------------------------------------------------------------------------------------------------------------
LZONE_FUNC
void intxx(void);



/****************************************************************************************************************//**
*   @fn                 void IdtSetHandler(int i, uint16_t sel, Addr_t handler, int ist, int dpl)
*   @brief              Set a handler in the IDT
*
*   This function will interact directly with the IDT, setting the handlers therein.
*
*   @param              i                   The interrupt number to set
*   @param              sel                 The segment selector for the handler when invoked
*   @param              handler             The handler long address location
*   @param              ist                 The Interrupt Stack to use (0 = Legacy Stack method)
*   @param              dpl                 The Descriptor Privilege Level
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void IdtSetHandler(int i, uint16_t sel, Addr_t handler, int ist, int dpl);



/****************************************************************************************************************//**
*   @fn                 void ArchEarlyInit(void)
*   @brief              Complete the early initialization tasks for the x86_64 arch
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ArchEarlyInit(void);



/****************************************************************************************************************//**
*   @fn                 void ArchApInit(void)
*   @brief              Complete the early initialization for the AP cores on x86_64 arch
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ArchApInit(void);


/****************************************************************************************************************//**
*   @fn                 void MoveTrampoline(void)
*   @brief              Move the trampoline code the its target location in 16-bit real mode address space
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void MoveTrampoline(void);



/****************************************************************************************************************//**
*   @fn                 void kInitAp(void)
*   @brief              Perform the initialization of the AP, prearing them for the kernel
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC NORETURN
void kInitAp(void);



/****************************************************************************************************************//**
*   @fn                 void POKE32(Addr_t regLocation, uint32_t data)
*   @brief              Write to a 32-bit Memory Mapped I/O Register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void POKE32(Addr_t regLocation, uint32_t data) {
    (*((volatile uint32_t *)(regLocation)) = (data));
}



/****************************************************************************************************************//**
*   @fn                 void POKE64(Addr_t regLocation, uint64_t data)
*   @brief              Write to a 64-bit Memory Mapped I/O Register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void POKE64(Addr_t regLocation, uint64_t data) {
    (*((volatile uint64_t *)(regLocation)) = (data));
}



/****************************************************************************************************************//**
*   @fn                 uint32_t PEEK32(Addr_t regLocation)
*   @brief              Read from a 32-bit Memory Mapped I/O Register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
uint32_t PEEK32(Addr_t regLocation) {
    return (*((volatile uint32_t *)(regLocation)));
}



/****************************************************************************************************************//**
*   @fn                 uint64_t PEEK64(Addr_t regLocation)
*   @brief              Read from a 64-bit Memory Mapped I/O Register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
uint64_t PEEK64(Addr_t regLocation) {
    return (*((volatile uint64_t *)(regLocation)));
}



/****************************************************************************************************************//**
*   @fn                 void LapicInit(void)
*   @brief              Perform the LAPIC initialization
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicInit(void);




/****************************************************************************************************************//**
*   @fn                 int LapicSendInit(int core)
*   @brief              Send an INIT IPI to a core
*
*   Send an INIT IPI to a core
*
*   @param              core                The core to receive the INIT IPI
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicSendInit(int core);



/****************************************************************************************************************//**
*   @fn                 int LapicSendSipi(int core, Addr_t vector)
*   @brief              Send a Startup IPI to a core
*
*   Send an Startup IPI to a core
*
*   @param              core                The core to receive the INIT IPI
*   @param              vector              The segment register (offset 0x0000) to set for the startup location
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicSendSipi(int core, Addr_t vector);



/****************************************************************************************************************//**
*   @fn                 int LapicGetId(void)
*   @brief              Read the Local APIC ID
*
*   Read the Local APIC ID, equivalent to the CPU number
*
*   @returns            Local APIC ID
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
int LapicGetId(void);



/****************************************************************************************************************//**
*   @fn                 void PlatformDiscovery(void)
*   @brief              Complete the hardware discovery for the platform
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void PlatformDiscovery(void);



#include "cpuid.h"
#include "msr.h"



/****************************************************************************************************************//**
*   @fn                 BOCHS_INSTRUMENTATION
*   @brief              Macro to toggle instrumentation in Bochs; will have no impact on anything else
*///-----------------------------------------------------------------------------------------------------------------
#define BOCHS_INSTRUMENTATION __asm volatile("xchg %edx,%edx");



/****************************************************************************************************************//**
*   @fn                 BOCHS_BREAK
*   @brief              Macro to set a breakpoint for Bochs; will have no impact on anything else
*///-----------------------------------------------------------------------------------------------------------------
#define BOCHS_BREAK __asm volatile("xchg %bx,%bx");




#include "cpu.h"



/****************************************************************************************************************//**
*   @fn                 int ThisCpuNum(void)
*   @brief              This this cpu's number
*///-----------------------------------------------------------------------------------------------------------------
INLINE
int ThisCpuNum(void) {
    Cpu_t *c;
    __asm volatile("mov %%gs:(8),%0" : "=r"(c) :: "memory");
    return c->cpuNumber;
}



#endif
