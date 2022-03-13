/****************************************************************************************************************//**
*   @file               msr.h
*   @brief              Functions and constants specific to Model-Specific Registers
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Mar-05
*   @since              v0.0.1
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#ifndef __MSR_H__
#define __MSR_H__



#ifndef __ARCH_H__
# error "Do not include 'msr.h' directly; include 'arch.h' instead"
#endif



/****************************************************************************************************************//**
*   @fn                 uint64_t RDMSR(uint32_t r)
*   @brief              Write to a 64-bit Model-Specific Register (MSR)
*///-----------------------------------------------------------------------------------------------------------------
INLINE
uint64_t RDMSR(uint32_t r) {
    uint64_t _lo, _hi;
    __asm volatile("rdmsr" : "=a"(_lo),"=d"(_hi) : "c"(r) : "%ebx", "memory");
    return (((uint64_t)_hi) << 32) | _lo;
}



/****************************************************************************************************************//**
*   @fn                 void WRMSR(uint32_t r, uint64_t v)
*   @brief              Write to a 64-bit Model-Specific Register (MSR)
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void WRMSR(uint32_t r, uint64_t v) {
    uint32_t _lo = (uint32_t)(v & 0xffffffff);
    uint32_t _hi = (uint32_t)(v >> 32);
    __asm volatile("wrmsr" : : "c"(r),"a"(_lo),"d"(_hi) : "memory");
}



/****************************************************************************************************************//**
*   @var                IA32_APIC_BASE_MSR
*   @brief              MSR location the APIC base
*///-----------------------------------------------------------------------------------------------------------------
const uint32_t IA32_APIC_BASE_MSR = 0x1b;




/****************************************************************************************************************//**
*   @var                IA32_KERNEL_GS_BASE
*   @brief              MSR location for swapping in a new gs
*///-----------------------------------------------------------------------------------------------------------------
const uint32_t IA32_KERNEL_GS_BASE = 0xc0000102;



#endif


