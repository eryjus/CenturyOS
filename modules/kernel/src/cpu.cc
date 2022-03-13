/****************************************************************************************************************//**
*   @file               cpu.cc
*   @brief              CPU abstractions
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Mar-01
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Mar-01 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================


#include "cpu.h"



/********************************************************************************************************************
*   See `cpu.h` for documentation
*///----------------------------------------------------------------------------------------------------------------
KERNEL_BSS
Cpu_t cpus[MAX_CPU];



/********************************************************************************************************************
*   See `cpu.h` for documentation
*///----------------------------------------------------------------------------------------------------------------
KERNEL_BSS
int cpuCount = 0;



/********************************************************************************************************************
*   See `cpu.h` for documentation
*///----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void BpCpuInit(void)
{
    cpuCount = 0;                       // -- start with 0 so ACPI can count them properly

    cpus[0].cpu = &cpus[0];
    cpus[0].cpuNumber = 0;
    cpus[0].currentProcess = 0;
    cpus[0].fenced = false;
    cpus[0].isBP = false;               // -- will let the LAPIC make this determination
    cpus[0].status = CPU_NONE;          // -- will let LAPIC make this determination for the BP

    for (int i = 1; i < MAX_CPU; i ++) {
        cpus[i].cpu = &cpus[i];
        cpus[i].cpuNumber = i;
        cpus[i].currentProcess = 0;
        cpus[i].fenced = true;
        cpus[i].isBP = false;
        cpus[0].status = CPU_NONE;
    }
}



/********************************************************************************************************************
*   See `cpu.h` for documentation
*///----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ApStart(void)
{
    MoveTrampoline();
}

