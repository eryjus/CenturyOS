/****************************************************************************************************************//**
*   @file               pmm.cc
*   @brief              Physical Memory Manager
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-28
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Implementation of the Physical Memory Manager (PMM).
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
*   See documentation in pmm.h
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
Frame_t PmmAllocate(void)
{
    extern Frame_t earlyFrame;

    return earlyFrame ++;
}

