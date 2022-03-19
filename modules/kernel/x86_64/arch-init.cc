/****************************************************************************************************************//**
*   @file               arch-init.cc
*   @brief              x86_64 Specific Initialization
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-27
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Perform the x86_64 specific initialization.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-27 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"
#include "cpu.h"
#include "mmu.h"
#include "pmm.h"
#include "internals.h"



/****************************************************************************************************************//**
*   @def                TSSL32_GDT
*   @brief              A helper macro used to define the 64-bit TSS (lower entry)
*///-----------------------------------------------------------------------------------------------------------------
#define TSSL32_GDT(locn)   (                                    \
      ((uint64_t)(sizeof(Tss_t) - 1) & 0xffff) << 0             \
    | ((uint64_t)(locn) & 0xffff) << 16                         \
    | (((uint64_t)(locn) >> 16) & 0xff) << 32                   \
    | (uint64_t)0x9 << 40                                       \
    | (uint64_t)0 << 44                                         \
    | (uint64_t)0 << 45                                         \
    | (uint64_t)1 << 47                                         \
    | (((uint64_t)(sizeof(Tss_t) - 1) >> 16) & 0xf) << 48       \
    | (uint64_t)0 << 52                                         \
    | (uint64_t)0 << 53                                         \
    | (uint64_t)0 << 54                                         \
    | (uint64_t)0 << 55                                         \
    | (((uint64_t)(locn) >> 24) & 0xff) << 56                   \
)



/****************************************************************************************************************//**
*   @def                TSSU32_GDT
*   @brief              A helper macro used to define the 64-bit TSS (upper entry)
*///-----------------------------------------------------------------------------------------------------------------
#define TSSU32_GDT(locn)       (                                \
      (uint64_t)(locn >> 32)                                    \
    | (uint64_t)0                                               \
)



/********************************************************************************************************************
*   See `arch.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ArchEarlyInit(void)
{
    extern uint64_t gdtFinal[];

    IdtSetHandler( 0, 0x08, (Addr_t)int00, 0, 0);
    IdtSetHandler( 1, 0x08, (Addr_t)int01, 0, 0);
    IdtSetHandler( 2, 0x08, (Addr_t)int02, 0, 0);
    IdtSetHandler( 3, 0x08, (Addr_t)int03, 0, 0);
    IdtSetHandler( 4, 0x08, (Addr_t)int04, 0, 0);
    IdtSetHandler( 5, 0x08, (Addr_t)int05, 0, 0);
    IdtSetHandler( 6, 0x08, (Addr_t)int06, 0, 0);
    IdtSetHandler( 7, 0x08, (Addr_t)int07, 0, 0);
    IdtSetHandler( 8, 0x08, (Addr_t)int08, 0, 0);
    IdtSetHandler( 9, 0x08, (Addr_t)int09, 0, 0);
    IdtSetHandler(10, 0x08, (Addr_t)int0a, 0, 0);
    IdtSetHandler(11, 0x08, (Addr_t)int0b, 0, 0);
    IdtSetHandler(12, 0x08, (Addr_t)int0c, 0, 0);
    IdtSetHandler(13, 0x08, (Addr_t)int0d, 0, 0);
    IdtSetHandler(14, 0x08, (Addr_t)int0e, 0, 0);
    IdtSetHandler(15, 0x08, (Addr_t)int0f, 0, 0);
    IdtSetHandler(16, 0x08, (Addr_t)int10, 0, 0);
    IdtSetHandler(17, 0x08, (Addr_t)int11, 0, 0);
    IdtSetHandler(18, 0x08, (Addr_t)int12, 0, 0);
    IdtSetHandler(19, 0x08, (Addr_t)int13, 0, 0);
    IdtSetHandler(20, 0x08, (Addr_t)int14, 0, 0);
    IdtSetHandler(21, 0x08, (Addr_t)int15, 0, 0);
    IdtSetHandler(22, 0x08, (Addr_t)int16, 0, 0);
    IdtSetHandler(23, 0x08, (Addr_t)int17, 0, 0);
    IdtSetHandler(24, 0x08, (Addr_t)int18, 0, 0);
    IdtSetHandler(25, 0x08, (Addr_t)int19, 0, 0);
    IdtSetHandler(26, 0x08, (Addr_t)int1a, 0, 0);
    IdtSetHandler(27, 0x08, (Addr_t)int1b, 0, 0);
    IdtSetHandler(28, 0x08, (Addr_t)int1c, 0, 0);
    IdtSetHandler(29, 0x08, (Addr_t)int1d, 0, 0);
    IdtSetHandler(30, 0x08, (Addr_t)int1e, 0, 0);
    IdtSetHandler(31, 0x08, (Addr_t)int1f, 0, 0);

    for (int i = 32; i < 256; i ++) {
        IdtSetHandler(i, 0x08, (Addr_t)intxx, 0, 0);
    }


    // -- go back and install the problem IRQ handlers
    IdtSetHandler(32, 0x08, (Addr_t)int20, 0, 0);

    //
    // -- Now, we need to establish the `gs` segment and the tss for this CPU
    //    -------------------------------------------------------------------
    gdtFinal[0xa0>>3] = TSSL32_GDT((Addr_t)&cpus[0].arch.tss);
    gdtFinal[0xa8>>3] = TSSU32_GDT((Addr_t)&cpus[0].arch.tss);
    LTR(0xa0);

    asm __volatile ("mov %0,%%gs" :: "r"(0x98) : "memory");
    WRMSR(IA32_GS_BASE, 0);
    WRMSR(IA32_KERNEL_GS_BASE, (Addr_t)cpus[0].cpu);
    SWAPGS();

    LapicInit();
}



/********************************************************************************************************************
*   See `arch.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ArchApInit(void)
{
    extern uint64_t gdtFinal[];
    int apicId = LapicGetId();


    //
    // -- Now, we need to establish the `gs` segment and the tss for this CPU
    //    -------------------------------------------------------------------
    asm __volatile ("mov %0,%%gs" :: "r"(0x98 + (apicId * 24)) : "memory");
    WRMSR(IA32_GS_BASE, 0);
    WRMSR(IA32_KERNEL_GS_BASE, (Addr_t)cpus[apicId].cpu);
    SWAPGS();

    gdtFinal[(0xa0>>3) + (apicId * 3)] = TSSL32_GDT((Addr_t)&cpus[apicId].arch.tss);
    gdtFinal[(0xa8>>3) + (apicId * 3)] = TSSU32_GDT((Addr_t)&cpus[apicId].arch.tss);
    LTR(0xa0 + ((apicId * 3) << 3));

    LapicInit();
}



/********************************************************************************************************************
*   See `arch.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void MoveTrampoline(void)
{
    if (cpuCount == 1) return;
    Addr_t stackBase = 0xffffc00000000000;

    struct Tramp_t {
        uint64_t jumpCode;
        uint32_t apLock;
        uint32_t apPml4;
        uint64_t stack;
        uint64_t entryPoint;
    } __attribute__((packed)) *tramp = (struct Tramp_t *)TRAMP_OFF;

    extern uint8_t _smpStart[];
    extern uint8_t _smpEnd[];
    extern Addr_t pml4;

    MapPage(TRAMP_OFF, TRAMP_OFF >> 12, PG_KRN | PG_WRT);
    kMemMove(tramp, _smpStart, _smpEnd - _smpStart);

    for (int i = 1; i < cpuCount; i ++) {
        cpus[i].status = CPU_STARTING;

        tramp->apLock = 0;
        tramp->apPml4 = pml4;
        tramp->stack = stackBase - (0x5000 * i);    // includes a guard page
        tramp->entryPoint = (Addr_t)kInitAp;

        // -- map the stack for the new CPU
        for (Addr_t s = tramp->stack - 0x4000; s < tramp->stack; s += 0x1000) {
            MapPage(s, PmmAllocate(), PG_KRN | PG_WRT);
        }

        LapicSendInit(i);
        LapicSendSipi(i, TRAMP_OFF);

        while (cpus[i].status == CPU_STARTING) {
            // -- TODO: check for timeout and fail CPU Startup
        }
    }
}


