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
*   Handle interacting with the serial port for the pc platform.
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


enum {
    PG_WRT = 0x00000001,                //!< VMM page is writable
    PG_DEV = 0x00000002,                //!< VMM page is not cacheable and it supervisor
    PG_KRN = 0x00000004,                //!< VMM Page is supervisor and is not swapable (but may be not present)
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
    unsigned int offsetLo : 16;                 //!< Bits 0-15 of the target handler address
    unsigned int segmentSel : 16;               //!< The `cs` segment selector of the handler
    unsigned int ist : 3;                       //!< The Index of the Stack to use for this handler (0=Legacy mode)
    unsigned int zero : 5;                      //!< These bits are set to 0
    unsigned int type : 4;                      //!< The handler type (0xe for Interrupt; 0xf for Trap)
    unsigned int off : 1;                       //!< This bit is set to 0
    unsigned int dpl : 2;                       //!< Descriptor Privilege Level
    unsigned int p : 1;                         //!< Present
    unsigned int offsetMid : 16;                //!< Bits 16-31 of the target handler address
    unsigned int offsetHi : 32;                 //!< Bits 32-63 of the target handler address
    unsigned int unused : 32;                   //!< Unused; set to 0
} __attribute__((packed)) IdtDescriptor_t;


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
    unsigned int p : 1;                         //!< Is the page present?
    unsigned int rw : 1;                        //!< set to 1 to allow writes
    unsigned int us : 1;                        //!< 0=Supervisor; 1=user
    unsigned int pwt : 1;                       //!< Page Write Through
    unsigned int pcd : 1;                       //!< Page-level cache disable
    unsigned int a : 1;                         //!< accessed
    unsigned int d : 1;                         //!< dirty (needs to be written for a swap)
    unsigned int pat : 1;                       //!< set to 0 for tables, page Page Attribute Table (set to 0)
    unsigned int g : 1;                         //!< Global (set to 0)
    unsigned int k : 1;                         //!< Is this a kernel-space (protected) page?
    unsigned int avl : 2;                       //!< Available for software use
    Frame_t frame : 36;                         //!< This is the 4K aligned page frame address (or table address)
    unsigned int reserved : 4;                  //!< reserved bits
    unsigned int software : 11;                 //!< software use bits
    unsigned int xd : 1;                        //!< execute disable
} __attribute__((packed)) PageEntry_t;



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
    uint32_t reserved0;                         //!< Unused
    uint32_t lowerRsp0;                         //!< Lower Stack Pointer to use when interrupting from ring 0
    uint32_t upperRsp0;                         //!< Upper Stack Pointer to use when interrupting from ring 0
    uint32_t lowerRsp1;                         //!< Lower Stack Pointer to use when interrupting from ring 1
    uint32_t upperRsp1;                         //!< Upper Stack Pointer to use when interrupting from ring 1
    uint32_t lowerRsp2;                         //!< Lower Stack Pointer to use when interrupting from ring 2
    uint32_t upperRsp2;                         //!< Upper Stack Pointer to use when interrupting from ring 2
    uint32_t lowerRsp3;                         //!< Lower Stack Pointer to use when interrupting from ring 3
    uint32_t upperRsp3;                         //!< Upper Stack Pointer to use when interrupting from ring 2
    uint32_t reserved1;                         //!< Unused
    uint32_t reserved2;                         //!< Unused
    uint32_t lowerIst1;                         //!< Lower Stack Pointer to use when IST1 is specified
    uint32_t upperIst1;                         //!< Upper Stack Pointer to use when IST1 is specified
    uint32_t lowerIst2;                         //!< Lower Stack Pointer to use when IST2 is specified
    uint32_t upperIst2;                         //!< Upper Stack Pointer to use when IST2 is specified
    uint32_t lowerIst3;                         //!< Lower Stack Pointer to use when IST3 is specified
    uint32_t upperIst3;                         //!< Upper Stack Pointer to use when IST3 is specified
    uint32_t lowerIst4;                         //!< Lower Stack Pointer to use when IST4 is specified
    uint32_t upperIst4;                         //!< Upper Stack Pointer to use when IST4 is specified
    uint32_t lowerIst5;                         //!< Lower Stack Pointer to use when IST5 is specified
    uint32_t upperIst5;                         //!< Upper Stack Pointer to use when IST5 is specified
    uint32_t lowerIst6;                         //!< Lower Stack Pointer to use when IST6 is specified
    uint32_t upperIst6;                         //!< Upper Stack Pointer to use when IST6 is specified
    uint32_t lowerIst7;                         //!< Lower Stack Pointer to use when IST7 is specified
    uint32_t upperIst7;                         //!< Upper Stack Pointer to use when IST7 is specified
    uint32_t reserved3;                         //!< Unused
    uint32_t reserved4;                         //!< Unused
    uint16_t reserved5;                         //!< Unused
    uint16_t ioMapBase;                         //!< Base offset to the start of the I/O permissions map
} __attribute__((packed)) Tss_t;



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
    Tss_t tss;                                  //!< This is the x86_64 TSS
} ArchCpu_t;



/****************************************************************************************************************//**
*   @def                MAX_CPU
*   @brief              The maximum number of CPUs supported on this architecture
*///----------------------------------------------------------------------------------------------------------------
#define MAX_CPU         4



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPml4Entry(Addr_t a)
*   @brief              Get the address of the PML4 Table Entry
*
*   @param              a                   The address for which to extract the PML4 Entry
*
*   @returns            A pointer to the PML4 Entry
*///-----------------------------------------------------------------------------------------------------------------
inline PageEntry_t *GetPml4Entry(Addr_t a) { return &((PageEntry_t *)0xfffffffffffff000)[(a >> 39) & 0x1ff]; }



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPdptEntry(Addr_t a)
*   @brief              Get the address of the PDPT Table Entry
*
*   @param              a                   The address for which to extract the PDPT Entry
*
*   @returns            A pointer to the PDPT Entry
*///-----------------------------------------------------------------------------------------------------------------
inline PageEntry_t *GetPdptEntry(Addr_t a) { return &((PageEntry_t *)0xffffffffffe00000)[(a >> 30) & 0x3ffff]; }



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPdEntry(Addr_t a)
*   @brief              Get the address of the PD Table Entry
*
*   @param              a                   The address for which to extract the PD Entry
*
*   @returns            A pointer to the PD Entry
*///-----------------------------------------------------------------------------------------------------------------
inline PageEntry_t *GetPdEntry(Addr_t a) { return &((PageEntry_t *)0xffffffffc0000000)[(a >> 21) & 0x7ffffff]; }



/****************************************************************************************************************//**
*   @fn                 PageEntry_t *GetPtEntry(Addr_t a)
*   @brief              Get the address of the PT Table Entry
*
*   @param              a                   The address for which to extract the PT Entry
*
*   @returns            A pointer to the PT Entry
*///-----------------------------------------------------------------------------------------------------------------
inline PageEntry_t *GetPtEntry(Addr_t a) { return &((PageEntry_t *)0xffffff8000000000)[(a >> 12) & 0xfffffffff]; }



/****************************************************************************************************************//**
*   @fn                 void ArchMmuMapPage(Addr_t a, Frame_t f, int flags)
*   @brief              Map a page in Virtual Memory Space to a Physical Frame
*
*   @param              a                   The address for which to map
*   @param              f                   The frame to which to map the page
*   @param              flags               Flags which will help control the mapping security
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void ArchMmuMapPage(Addr_t a, Frame_t f, int flags);



/****************************************************************************************************************//**
*   @fn                 uint8_t INB(uint16_t port)
*   @brief              Get a byte from an I/O Port
*
*   @param              port                The I/O port to read
*
*   @returns            The byte read from the I/O port
*///-----------------------------------------------------------------------------------------------------------------
inline uint8_t INB(uint16_t port) {
    uint8_t rv;
    __asm volatile ( "inb %1, %0" : "=a"(rv) : "Nd"(port) );
    return rv;
}


/****************************************************************************************************************//**
*   @fn                 void INVLPG(Addr_t a)
*   @brief              Invalidate a page from the TLB
*///-----------------------------------------------------------------------------------------------------------------
inline void INVLPG(Addr_t a) {
    __asm volatile("invlpg (%0)" :: "r"(a) : "memory");
}



/****************************************************************************************************************//**
*   @fn                 void LTR(uint16_t tr)
*   @brief              Load the task register
*///-----------------------------------------------------------------------------------------------------------------
inline void LTR(uint16_t tr) {
    __asm volatile("ltr  %0" :: "r"(tr) : "memory");
}

/****************************************************************************************************************//**
*   @fn                 void OUTB(uint16_t port, uint8_t val)
*   @brief              Write a byte to an I/O Port
*
*   @param              port                The I/O port to which to write
*   @param              val                 The value to write to the I/O port
*///-----------------------------------------------------------------------------------------------------------------
inline void OUTB(uint16_t port, uint8_t val)
{
    __asm volatile ( "outb %0, %1" :: "a"(val), "Nd"(port) );
}



/****************************************************************************************************************//**
*   @fn                 void SWAPGS(void)
*   @brief              Swap the `gs` register with the IA32_KERNEL_GS_BASE model-specific register (setting limits)
*///-----------------------------------------------------------------------------------------------------------------
inline void SWAPGS(void) {
    __asm volatile ("swapgs" ::: "memory");
}


/****************************************************************************************************************//**
*   @fn                 void WBNOINVD(void)
*   @brief              Synchronize the cpu caches
*///-----------------------------------------------------------------------------------------------------------------
inline void WBNOINVD(void) {
    __asm volatile ("wbnoinvd" ::: "memory");
}



/****************************************************************************************************************//**
*   @fn                 void WRMSR(uint32_t r, uint64_t v)
*   @brief              Write to a 32-bit Model-Specific Register (MSR)
*///-----------------------------------------------------------------------------------------------------------------
inline void WRMSR(uint32_t r, uint64_t v) {
    uint32_t _lo = (uint32_t)(v & 0xffffffff);
    uint32_t _hi = (uint32_t)(v >> 32);
    __asm volatile("wrmsr" : : "c"(r),"a"(_lo),"d"(_hi) : "memory");
}



/****************************************************************************************************************//**
*   @var                IA32_KERNEL_GS_BASE
*   @brief              MSR location for swapping in a new gs
*///-----------------------------------------------------------------------------------------------------------------
const uint32_t IA32_KERNEL_GS_BASE = 0xc0000102;



/****************************************************************************************************************//**
*   @fn                 void int00(void)
*   @brief              Handle the \#DE Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int00(void);


/****************************************************************************************************************//**
*   @fn                 void int01(void)
*   @brief              Handle a \#DB Fault/Trap
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int01(void);


/****************************************************************************************************************//**
*   @fn                 void int02(void)
*   @brief              Handle a \#MNI Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int02(void);


/****************************************************************************************************************//**
*   @fn                 void int03(void)
*   @brief              Handle a \#BP Trap
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int03(void);


/****************************************************************************************************************//**
*   @fn                 void int04(void)
*   @brief              Handle a \#OF Trap
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int04(void);


/****************************************************************************************************************//**
*   @fn                 void int05(void)
*   @brief              Handle a \#BR Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int05(void);


/****************************************************************************************************************//**
*   @fn                 void int06(void)
*   @brief              Handle a \#UD Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int06(void);


/****************************************************************************************************************//**
*   @fn                 void int07(void)
*   @brief              Handle a \#NM Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int07(void);


/****************************************************************************************************************//**
*   @fn                 void int08(void)
*   @brief              Handle a \#DF Abort
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int08(void);


/****************************************************************************************************************//**
*   @fn                 void int09(void)
*   @brief              Legacy Coprocessor Segment Overrun
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int09(void);


/****************************************************************************************************************//**
*   @fn                 void int0a(void)
*   @brief              Handle a \#TS Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int0a(void);


/****************************************************************************************************************//**
*   @fn                 void int0b(void)
*   @brief              Handle a \#NP Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int0b(void);


/****************************************************************************************************************//**
*   @fn                 void int0c(void)
*   @brief              Handle a \#SS Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int0c(void);


/****************************************************************************************************************//**
*   @fn                 void int0d(void)
*   @brief              Handle a \#GP Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int0d(void);


/****************************************************************************************************************//**
*   @fn                 void int0e(void)
*   @brief              Handle a \#PF Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int0e(void);


/****************************************************************************************************************//**
*   @fn                 void int0f(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int0f(void);


/****************************************************************************************************************//**
*   @fn                 void int10(void)
*   @brief              Handle a \#MF Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int10(void);


/****************************************************************************************************************//**
*   @fn                 void int11(void)
*   @brief              Handle an \#AC Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int11(void);


/****************************************************************************************************************//**
*   @fn                 void int12(void)
*   @brief              Handle an \#MC Abort
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int12(void);


/****************************************************************************************************************//**
*   @fn                 void int13(void)
*   @brief              Handle an \#XM Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int13(void);


/****************************************************************************************************************//**
*   @fn                 void int14(void)
*   @brief              Handle an \#VE Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int14(void);


/****************************************************************************************************************//**
*   @fn                 void int15(void)
*   @brief              Handle an \#CP Fault
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int15(void);


/****************************************************************************************************************//**
*   @fn                 void int16(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int16(void);


/****************************************************************************************************************//**
*   @fn                 void int17(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int17(void);


/****************************************************************************************************************//**
*   @fn                 void int18(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int18(void);


/****************************************************************************************************************//**
*   @fn                 void int19(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int19(void);


/****************************************************************************************************************//**
*   @fn                 void int1a(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int1a(void);


/****************************************************************************************************************//**
*   @fn                 void int1b(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int1b(void);


/****************************************************************************************************************//**
*   @fn                 void int1c(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int1c(void);


/****************************************************************************************************************//**
*   @fn                 void int1d(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int1d(void);


/****************************************************************************************************************//**
*   @fn                 void int1e(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int1e(void);


/****************************************************************************************************************//**
*   @fn                 void int1f(void)
*   @brief              Reserved Interrupt
*///-----------------------------------------------------------------------------------------------------------------
extern "C" __attribute__((noreturn)) void int1f(void);


/****************************************************************************************************************//**
*   @fn                 void intxx(void)
*   @brief              Generically handle any other IRQ or software generated interrupt
*
*   @note               This handler will be used as a stopgap for all other interrupts.  Specific handlers will be
*                       installed to handle specific things.
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void intxx(void);


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
extern "C" void IdtSetHandler(int i, uint16_t sel, Addr_t handler, int ist, int dpl);


/****************************************************************************************************************//**
*   @fn                 void ArchEarlyInit(void)
*   @brief              Complete the early initialization tasks for the x86_64 arch
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void ArchEarlyInit(void);


#endif
