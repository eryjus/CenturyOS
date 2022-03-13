/****************************************************************************************************************//**
*   @file               arch-mmu.cc
*   @brief              Handle the x86_64 MMU interface
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-28
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   This file handles all the intricacies of the x86_64 MMU interface.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-28 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"
#include "pmm.h"



/********************************************************************************************************************
*   See documentation in arch.h
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ArchMmuMapPage(Addr_t a, Frame_t f, int flags)
{
    Frame_t t;
    PageEntry_t *ent = GetPml4Entry(a);

    if (!ent->p) {
        t = PmmAllocate();
        ent->frame = t;
        ent->rw = 1;
        ent->p = 1;

        WBNOINVD();
        INVLPG((Addr_t)GetPdptEntry(a));

        uint64_t *tbl = (uint64_t *)((Addr_t)GetPdptEntry(a) & 0xfffffffffffff000);
        for (int i = 0; i < 512; i ++) {
            tbl[i] = 0;
        }
    }

    ent = GetPdptEntry(a);

    if (!ent->p) {
        t = PmmAllocate();
        ent->frame = t;
        ent->rw = 1;
        ent->p = 1;

        WBNOINVD();
        INVLPG((Addr_t)GetPdEntry(a));

        uint64_t *tbl = (uint64_t *)((Addr_t)GetPdEntry(a) & 0xfffffffffffff000);
        for (int i = 0; i < 512; i ++) tbl[i] = 0;
    }

    ent = GetPdEntry(a);

    if (!ent->p) {
        t = PmmAllocate();
        ent->frame = t;
        ent->rw = 1;
        ent->p = 1;

        WBNOINVD();
        INVLPG((Addr_t)GetPtEntry(a));

        uint64_t *tbl = (uint64_t *)((Addr_t)GetPtEntry(a) & 0xfffffffffffff000);
        for (int i = 0; i < 512; i ++) tbl[i] = 0;
    }

    ent = GetPtEntry(a);

    ent->frame = f;
    ent->rw = (flags&PG_WRT?1:0);
    ent->pcd = (flags&PG_DEV?1:0);
    ent->pwt = (flags&PG_DEV?1:0);
    ent->us = ((flags&PG_DEV)||(flags&PG_KRN)?1:0);
    ent->k = (flags&PG_KRN?1:0);
    ent->g = (flags&PG_KRN?1:0);
    ent->p = 1;

    WBNOINVD();
    INVLPG((Addr_t)a);
}






