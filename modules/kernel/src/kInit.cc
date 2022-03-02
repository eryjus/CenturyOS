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



#include "internals.h"
#include "cpu.h"


/********************************************************************************************************************
*   See `internals.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void EarlyInit(void)
{
    CpuInit();
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
extern "C" __attribute__((noreturn)) void kInit(void)
{
    EarlyInit();
    DbgPrintf("Hello, World!\n");

    while (true) {}
}

