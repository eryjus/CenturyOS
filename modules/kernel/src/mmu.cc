/****************************************************************************************************************//**
*   @file               mmu.cc
*   @brief              Handle the general MMU interface
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-28
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   This file handles the MMU interface to the rest of the kernel
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-28 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"



/********************************************************************************************************************
*   See documentation in mmu.h
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void MapPage(Addr_t a, Frame_t f, int flags)
{
    if (a == 0) return;

    ArchMmuMapPage(a, f, flags);
}


