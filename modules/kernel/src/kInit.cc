/****************************************************************************************************************//**
*   @file               kInit.cc
*   @brief              Kernel Initialization
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Perform the kenrel initialization.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"
#include "internals.h"
#include "cpu.h"


/********************************************************************************************************************
*   See `internals.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
EXTERNC void EarlyInit(void)
{
    BpCpuInit();
    ArchEarlyInit();
}



/****************************************************************************************************************//**
*   @fn                 void kInit(void)
*   @brief              Perform the kernel initialization
*
*   Complete the kernel initialization.
*
*   @note               This function does not return.
*///-----------------------------------------------------------------------------------------------------------------
EXTERNC NORETURN
void kInit(void)
{
    EarlyInit();
    DbgPrintf("Hello, World!\n");

    PlatformDiscovery();

    ApStart();

    EnableInterrupts();

    while (true) {}
}



/********************************************************************************************************************
*   See documentation in `arch.h`
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC NORETURN
void kInitAp(void)
{
    ArchApInit();

    DbgPrintf("Hello, World from CPU%d\n", LapicGetId());
    cpus[LapicGetId()].status = CPU_FENCED;

    EnableInterrupts();

    // -- Hold this CPU here until it is released to start scheduling
    while (cpus[LapicGetId()].status == CPU_FENCED) {}

    // -- Currently will never get here
    while (true){}
}

