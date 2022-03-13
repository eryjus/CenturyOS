/****************************************************************************************************************//**
*   @file               pmm.h
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



#ifndef __PMM_H__
#define __PMM_H__



#include "arch.h"



/****************************************************************************************************************//**
*   @fn                 Frame_t PmmAllocate(void)
*   @brief              Allocate a frame
*
*   @returns            A frame number which has now been allocated to the requestor.
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
Frame_t PmmAllocate(void);


#endif

