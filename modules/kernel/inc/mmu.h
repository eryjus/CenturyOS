/****************************************************************************************************************//**
*   @file               mmu.h
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



#ifndef __MMU_H__
#define __MMU_H__



#include "arch.h"



/****************************************************************************************************************//**
*   @fn                 void MapPage(Addr_t a, Frame_t f, int flags)
*   @brief              Map a page in Virtual Memory Space to a Physical Frame
*
*   @param              a                   The address for which to map
*   @param              f                   The frame to which to map the page
*   @param              flags               Flags which will help control the mapping security
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void MapPage(Addr_t a, Frame_t f, int flags);



#endif
