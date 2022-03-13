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



/********************************************************************************************************************
*   Some constants for the CPU status for each core
*///----------------------------------------------------------------------------------------------------------------
enum {
    CPU_NONE = 0,                   //!< The CPU is not known to exist
    CPU_RUNNING = 1,                //!< The CPU is running normally
    CPU_FENCED = 2,                 //!< The CPU is fenced and waiting to be released
    CPU_STARTING = 3,               //!< The CPU is attempting to start, but has not yet reached a known good state
    CPU_IDLE = 4,                   //!< The CPU is idle and may be powered off
    CPU_OFF = 5,                    //!< The CPU is powered off to save energy
};



/****************************************************************************************************************//**
*   @typedef            Cpu_t
*   @brief              Formalization of the \ref Cpu_t structure into a defined type
*///----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             Cpu_t
*   @brief              Abstraction for the CPU
*///----------------------------------------------------------------------------------------------------------------
typedef struct Cpu_t {
    volatile Addr_t currentProcess;             //!< The current process running on this CPU
    struct Cpu_t *cpu;                          //!< Self-pointer
    int cpuNumber;                              //!< The CPU Number
    bool isBP;                                  //!< Is this the boot processor?
    volatile bool fenced;                       //!< Has this CPU been fenced and held?
    volatile int status;                        //!< The current CPU Status
    ArchCpu_t arch;                             //!< Architecture-specific data elements
} Cpu_t;



/****************************************************************************************************************//**
*   @var                cpus
*   @brief              CPU abstraction structure for each CPU in this Arch
*///----------------------------------------------------------------------------------------------------------------
extern KERNEL_BSS
Cpu_t cpus[MAX_CPU];



/****************************************************************************************************************//**
*   @var                cpuCount
*   @brief              The number of CPUs discovered on this system
*///----------------------------------------------------------------------------------------------------------------
extern KERNEL_BSS
int cpuCount;



/****************************************************************************************************************//**
*   @fn                 void BpCpuInit(void)
*   @brief              Perform the CPU initialization
*///----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void BpCpuInit(void);


/****************************************************************************************************************//**
*   @fn                 void ApStart(void)
*   @brief              Start any AP CPUs
*///----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void ApStart(void);



#endif
