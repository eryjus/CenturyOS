/****************************************************************************************************************//**
*   @file               cpu.h
*   @brief              These structures are for managing the CPU abstraction structures
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


#ifndef __CPU_H__
#define __CPU_H__



#include "arch.h"



/****************************************************************************************************************//**
*   @typedef            Cpu_t
*   @brief              Formalization of the \ref Cpu_t structure into a defined type
*///----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             Cpu_t
*   @brief              Abstraction for the CPU
*///----------------------------------------------------------------------------------------------------------------
typedef struct Cpu_t {
    Addr_t currentProcess;                      //!< The current process running on this CPU
    struct Cpu_t *cpu;                          //!< Self-pointer
    int cpuNumber;                              //!< The CPU Number
    bool fenced;                                //!< Has this CPU been fenced and held?
    ArchCpu_t arch;                             //!< Architecture-specific data elements
} Cpu_t;



/****************************************************************************************************************//**
*   @var                cpus
*   @brief              CPU abstraction structure for each CPU in this Arch
*///----------------------------------------------------------------------------------------------------------------
extern Cpu_t cpus[MAX_CPU];



/****************************************************************************************************************//**
*   @fn                 void CpuInit(void)
*   @brief              Perform the CPU initialization
*///----------------------------------------------------------------------------------------------------------------
extern "C" void CpuInit(void);

#endif
