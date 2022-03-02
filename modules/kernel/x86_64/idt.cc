/****************************************************************************************************************//**
*   @file               idt.cc
*   @brief              Architecture-specific functions for handling the x86_64 IDT
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-27
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Interact with the x86_64 IDT
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-27 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"


/********************************************************************************************************************
*   See `arch.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void IdtSetHandler(int i, uint16_t sel, Addr_t handler, int ist, int dpl)
{
    IdtDescriptor_t desc;
    extern IdtDescriptor_t idtFinal[256];

    desc.offsetLo = handler & 0xffff;
    desc.segmentSel = sel;
    desc.ist = ist;
    desc.zero = 0;
    desc.type = 0xe;
    desc.off = 0;
    desc.dpl = dpl;
    desc.p = 1;
    desc.offsetMid = (handler >> 16) & 0xffff;
    desc.offsetHi = (handler >> 32) & 0xffffffff;
    desc.unused = 0;

    idtFinal[i] = desc;
}

