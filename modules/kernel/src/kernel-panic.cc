/****************************************************************************************************************//**
*   @file               kernel-panic.cc
*   @brief              Panic the kernel and stop all execution
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Panic the kernel and stop everything
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "internals.h"



/********************************************************************************************************************
*   See `internals.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC NORETURN
void KernelPanic(const char *msg)
{
    DbgPrintf(ANSI_BG_RED ANSI_FG_WHITE ANSI_ATTR_BOLD ANSI_CLEAR ANSI_SET_CURSOR(0,0));
    DbgPrintf("─────[ %-35s ]────────────────────────────────────────────────────────────────\n", msg);

    DbgPrintf(ANSI_ATTR_NORMAL);

    while (true) {}
}

