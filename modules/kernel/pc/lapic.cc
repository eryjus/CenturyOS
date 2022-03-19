/****************************************************************************************************************//**
*   @file               lapic.cc
*   @brief              Functions to handle the Local APIC (both XAPIC and x2APIC)
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
*   | 2022-Mar-05 | Initial |  v0.0.1  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"
#include "internals.h"
#include "mmu.h"
#include "cpu.h"



/****************************************************************************************************************//**
*   @enum               ApicVersion_t
*   @brief              Which kind of APIC are we talking about?
*///-----------------------------------------------------------------------------------------------------------------
enum ApicVersion_t {
    XNONE = 0,                  //!< Uninitialized
    XAPIC = 1,                  //!< Original XAPIC
    X2APIC = 2,                 //!< X2APIC
};



/****************************************************************************************************************//**
*   @enum               ApicRegister_t
*   @brief              The registers that can be read/written in the Local APIC
*///-----------------------------------------------------------------------------------------------------------------
enum ApicRegister_t {
    APIC_LOCAL_ID = 0x20,               //!< Local APIC ID Register
    APIC_LOCAL_VERSION = 0x30,          //!< Local APIC Version Register
    APIC_TPR = 0x80,                    //!< Task Priority Register
    APIC_APR = 0x90,                    //!< Arbitration Priority Register
    APIC_PPR = 0xa0,                    //!< Processor Priority Register
    APIC_EOI = 0xb0,                    //!< EOI Register
    APIC_RRD = 0xc0,                    //!< Remote Read Register
    APIC_LDR = 0xd0,                    //!< Logical Destination Reister
    APIC_DFR = 0xe0,                    //!< Destination Format Register
    APIC_SIVR = 0xf0,                   //!< Spurious Interrupt Vector Register
    APIC_ISR_BASE = 0x100,              //!< In-Service Register Base
    APIC_TMR_BASE = 0x180,              //!< Trigger Mode Register Base
    APIC_IRR_BASE = 0x200,              //!< Interrupt Request Register Base
    APIC_ESR = 0x280,                   //!< Error Status Register
    APIC_CMCI = 0x2f0,                  //!< LVT Corrected Machine Check Interrupt Register
    APIC_ICR1 = 0x300,                  //!< Interrupt Command Register 1 (low bits)
    APIC_ICR2 = 0x310,                  //!< Interrupt Command REgister 2 (high bits)
    APIC_LVT_TIMER = 0x320,             //!< LVT Timer Register
    APIC_LVT_THERMAL_SENSOR = 0x330,    //!< LVT Thermal Sensor Register
    APIC_LVT_PERF_COUNTING_REG = 0x340, //!< LVT Performance Monitoring Counters Register
    APIC_LVT_LINT0 = 0x350,             //!< LVT LINT0 Register
    APIC_LVT_LINT1 = 0x360,             //!< LVT LINT1 Register
    APIC_LVT_ERROR = 0x370,             //!< LVT Error Register
    APIC_TIMER_ICR = 0x380,             //!< Inital Count Register (for Timer)
    APIC_TIMER_CCR = 0x390,             //!< Current Count Register (for Timer)
    APIC_TIMER_DCR = 0x3e0,             //!< Divide Configuration Register (for Timer; X2APIC only)
    APIC_SELF_IPI = 0x3f0,              //!< SELF IPI (X2APIC only)
};



/****************************************************************************************************************//**
*   @def                APIC_SOFTWARE_ENABLE
*   @brief              APIC bit to software-enable the APIC Timer
*///-----------------------------------------------------------------------------------------------------------------
#define APIC_SOFTWARE_ENABLE            (1<<8)



/****************************************************************************************************************//**
*   @def                IA32_APIC_BASE_MSR__BSP
*   @brief              APIC bit to determine if core is boot processor
*///-----------------------------------------------------------------------------------------------------------------
#define IA32_APIC_BASE_MSR__BSP         (1<<8)



/****************************************************************************************************************//**
*   @def                IA32_APIC_BASE_MSR__EN
*   @brief              APIC bit to enable the APIC
*///-----------------------------------------------------------------------------------------------------------------
#define IA32_APIC_BASE_MSR__EN          (1<<11)



/****************************************************************************************************************//**
*   @def                IA32_APIC_BASE_MSR__EXTD
*   @brief              APIC bit to enable the x2APIC mode
*///-----------------------------------------------------------------------------------------------------------------
#define IA32_APIC_BASE_MSR__EXTD        (1<<10)



/****************************************************************************************************************//**
*   @def                APIC_LVT_MASKED
*   @brief              APIC bit mask an interrupt vector
*///-----------------------------------------------------------------------------------------------------------------
#define APIC_LVT_MASKED                 (1<<16)



/****************************************************************************************************************//**
*   @def                APIC_LVT_TIMER_PERIODIC
*   @brief              APIC bits to set the timer to periodic mode
*///-----------------------------------------------------------------------------------------------------------------
#define APIC_LVT_TIMER_PERIODIC         (0b01<<17)



/****************************************************************************************************************//**
*   @typedef            ApicOps_t
*   @brief              Formalization of the APIC Operations structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             ApicOps_t
*   @brief              The APIC Operations structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct ApicOps_t {
    uint64_t xApicBase;                                     //!< for XAPIC, the base address of the APIC
    ApicVersion_t version;                                  //!< Which APIC version?
    uint32_t (*readApicRegister)(ApicRegister_t);           //!< Read an APIC register
    void (*writeApicRegister)(ApicRegister_t, uint32_t);    //!< Write to an APIC register
    void (*writeApicIcr)(uint64_t);                         //!< Write to the APIC Interrupt Control Register
    bool (*checkIndexedStatus)(ApicRegister_t, uint8_t);    //!< Checked the status of an indexed register
    int (*getApicId)(void);                                 //!< Return the APIC ID
} ApicOps_t;



/****************************************************************************************************************//**
*   @var                apicOps
*   @brief              The APIC Operations with their functions filled in
*///-----------------------------------------------------------------------------------------------------------------
KERNEL_BSS
ApicOps_t apicOps;



/****************************************************************************************************************//**
*   @fn                 bool IsReadable(ApicRegister_t reg)
*   @brief              Is the APIC register a readable register?
*
*   Reports if the specified APIC register is a readable one.
*
*   @param              reg                 The APIC register in question
*
*   @returns            Whether the APIC register is a readable register
*
*   @retval             true                The APIC register is a readable register
*   @retval             false               The APIC register is not a readable register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool IsReadable(ApicRegister_t reg)
{
    switch (reg) {
        case APIC_LOCAL_ID:
        case APIC_LOCAL_VERSION:
        case APIC_TPR:
        case APIC_APR:
        case APIC_PPR:
        case APIC_RRD:
        case APIC_LDR:
        case APIC_DFR:
        case APIC_SIVR:
        case APIC_ESR:
        case APIC_CMCI:
        case APIC_ICR1:
        case APIC_LVT_TIMER:
        case APIC_LVT_THERMAL_SENSOR:
        case APIC_LVT_PERF_COUNTING_REG:
        case APIC_LVT_LINT0:
        case APIC_LVT_LINT1:
        case APIC_LVT_ERROR:
        case APIC_TIMER_ICR:
        case APIC_TIMER_CCR:
        case APIC_TIMER_DCR:
            return true;

        case APIC_ICR2:
            if (apicOps.version == XAPIC) return true;
            else return false;

        default:
            return false;
    }
}



/****************************************************************************************************************//**
*   @fn                 bool IsWritable(ApicRegister_t reg)
*   @brief              Is the APIC register a writable register?
*
*   Reports if the specified APIC register is a writable one.
*
*   @param              reg                 The APIC register in question
*
*   @returns            Whether the APIC register is a writable register
*
*   @retval             true                The APIC register is a writable register
*   @retval             false               The APIC register is not a writable register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool IsWritable(ApicRegister_t reg)
{
    switch (reg) {
        case APIC_TPR:
        case APIC_EOI:
        case APIC_SIVR:
        case APIC_ESR:
        case APIC_CMCI:
        case APIC_ICR1:
        case APIC_LVT_TIMER:
        case APIC_LVT_THERMAL_SENSOR:
        case APIC_LVT_PERF_COUNTING_REG:
        case APIC_LVT_LINT0:
        case APIC_LVT_LINT1:
        case APIC_LVT_ERROR:
        case APIC_TIMER_ICR:
        case APIC_TIMER_DCR:
            return true;

        case APIC_ICR2:
        case APIC_SELF_IPI:
            if (apicOps.version == XAPIC) return true;
            else return false;

        default:
            return false;
    }
}



/****************************************************************************************************************//**
*   @fn                 bool IsStatus(ApicRegister_t reg)
*   @brief              Is the APIC register a status register?
*
*   Reports if the specified APIC register is a status one.
*
*   @param              reg                 The APIC register in question
*
*   @returns            Whether the APIC register is a status register
*
*   @retval             true                The APIC register is a status register
*   @retval             false               The APIC register is not a status register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool IsStatus(ApicRegister_t reg)
{
    if (reg == APIC_ISR_BASE || reg == APIC_TMR_BASE || reg == APIC_IRR_BASE) return true;
    else return false;
}



/****************************************************************************************************************//**
*   @fn                 uint32_t GetX2apicMsr(ApicRegister_t reg)
*   @brief              Convert the APIC MMIO offset into a MSR number
*
*   Convert the APIC MMIO offset into a MSR number
*
*   @param              reg                 The APIC register in question
*
*   @returns            The contents of the register read; 0 if an invalid register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
uint32_t GetX2apicMsr(ApicRegister_t reg)
{
    return (uint32_t)(reg >> 4) + 0x800;
}



/****************************************************************************************************************//**
*   @fn                 uint32_t ReadX2apicRegister(ApicRegister_t reg)
*   @brief              Read an APIC register
*
*   Read an XAPIC register and return its contents
*
*   @param              reg                 The APIC register in question
*
*   @returns            The contents of the register read; 0 if an invalid register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
uint32_t ReadX2apicRegister(ApicRegister_t reg)
{
    if (!IsReadable(reg)) return 0;

    return RDMSR(GetX2apicMsr(reg));
}


/****************************************************************************************************************//**
*   @fn                 void WriteX2apicRegister(ApicRegister_t reg, uint32_t val)
*   @brief              Write an APIC register
*
*   Write a value to an XAPIC register
*
*   @param              reg                 The APIC register in question
*   @param              val                 The value to write to the register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void WriteX2apicRegister(ApicRegister_t reg, uint32_t val)
{
    if (!IsWritable(reg)) return;

    WRMSR(GetX2apicMsr(reg), val);
}



/****************************************************************************************************************//**
*   @fn                 bool CheckX2apicStatus(ApicRegister_t reg, uint8_t index)
*   @brief              Check APIC Status Register
*
*   Check an XAPIC status register and return if it is set
*
*   @param              reg                 The APIC register in question
*   @param              index               The index of the register status to check
*
*   @returns            If the status bit is set
*
*   @retval             false               The status bit is not set
*   @retval             true                The status but is set
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool CheckX2apicStatus(ApicRegister_t reg, uint8_t index)
{
    if (!IsStatus(reg)) return false;

    int off = index / 32;
    int bit = index % 32;

    return (RDMSR(GetX2apicMsr(reg) + off) & (1 << bit)) != 0;
}



/****************************************************************************************************************//**
*   @fn                 int X2apicGetId(void)
*   @brief              Get the APIC ID from the x2APIC
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
int X2apicGetId(void)
{
    return apicOps.readApicRegister(APIC_LOCAL_ID);
}



/****************************************************************************************************************//**
*   @fn                 uint32_t ReadXapicRegister(ApicRegister_t reg)
*   @brief              Read an APIC register
*
*   Read an XAPIC register and return its contents
*
*   @param              reg                 The APIC register in question
*
*   @returns            The contents of the register read; 0 if an invalid register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
uint32_t ReadXapicRegister(ApicRegister_t reg)
{
    if (!IsReadable(reg)) return 0;

    return PEEK32(apicOps.xApicBase + reg);
}



/****************************************************************************************************************//**
*   @fn                 void WriteXapicRegister(ApicRegister_t reg, uint32_t val)
*   @brief              Write an APIC register
*
*   Write a value to an XAPIC register
*
*   @param              reg                 The APIC register in question
*   @param              val                 The value to write to the register
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void WriteXapicRegister(ApicRegister_t reg, uint32_t val)
{
    if (!IsWritable(reg)) return;

    POKE32(apicOps.xApicBase + reg, val);
}



/****************************************************************************************************************//**
*   @fn                 bool CheckXapicStatus(ApicRegister_t reg, uint8_t index)
*   @brief              Check APIC Status Register
*
*   Check an XAPIC status register and return if it is set
*
*   @param              reg                 The APIC register in question
*   @param              index               The index of the register status to check
*
*   @returns            If the status bit is set
*
*   @retval             false               The status bit is not set
*   @retval             true                The status but is set
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool CheckXapicStatus(ApicRegister_t reg, uint8_t index)
{
    if (!IsStatus(reg)) return false;

    int off = index / 32;
    int bit = index % 32;

    return (PEEK32(apicOps.xApicBase + reg + off) & (1 << bit)) != 0;
}



/****************************************************************************************************************//**
*   @fn                 void WriteX2apicIcr(uint64_t val)
*   @brief              Write 64-bits to the ICR register
*
*   Write 64-bits to the ICR register
*
*   @param              val                 The value to write to the ICR register
*///-----------------------------------------------------------------------------------------------------------------
INLINE
void WriteX2apicIcr(uint64_t val)
{
    WriteX2apicRegister(APIC_ICR2, val);
}


/****************************************************************************************************************//**
*   @fn                 void WriteXapicIcr(uint64_t val)
*   @brief              Write 64-bits to the ICR register
*
*   Write 64-bits to the ICR register
*
*   @param              val                 The value to write to the ICR register
*///-----------------------------------------------------------------------------------------------------------------
void WriteXapicIcr(uint64_t val)
{
    uint32_t hi = (uint32_t)((val >> 32) & 0xffffffff);
    uint32_t lo = (uint32_t)(val & 0xffffffff);

    WriteXapicRegister(APIC_ICR2, hi);
    WriteXapicRegister(APIC_ICR1, lo);
}


/****************************************************************************************************************//**
*   @fn                 int XapicGetId(void)
*   @brief              Get the APIC ID from the x2APIC
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
int XapicGetId(void)
{
    return apicOps.readApicRegister(APIC_LOCAL_ID) >> 24;
}



/****************************************************************************************************************//**
*   see arch.h for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicInit(void)
{
    uint32_t eax, ebx, ecx, edx;
    CPUID(0x01, &eax, &ebx, &ecx, &edx);

    if (!(edx & CPUID_FEAT_EDX_APIC)) KernelPanic("Unable to find a suitable APIC timer.");

    bool isX2 = ((ecx & CPUID_FEAT_ECX_X2APIC) != 0);

    static int freq = 1000;
    static uint64_t factor = ~0;
    uint32_t apicId;

    uint64_t apicBaseMsr = RDMSR(IA32_APIC_BASE_MSR);
    bool isBoot = (apicBaseMsr & IA32_APIC_BASE_MSR__BSP) != 0;

    if (isX2) {
        apicOps.version = X2APIC;
        apicOps.readApicRegister = ReadX2apicRegister;
        apicOps.writeApicRegister = WriteX2apicRegister;
        apicOps.checkIndexedStatus = CheckX2apicStatus;
        apicOps.writeApicIcr = WriteX2apicIcr;
        apicOps.getApicId = X2apicGetId;

        if (isBoot) {
            // -- enable the APIC and x2apic mode
            WRMSR(IA32_APIC_BASE_MSR, 0
                    | IA32_APIC_BASE_MSR__EN
                    | IA32_APIC_BASE_MSR__EXTD
                    | (apicBaseMsr & ~(PAGE_SIZE-1)));

            apicId = apicOps.readApicRegister(APIC_LOCAL_ID);

            if (apicId > MAX_CPU) KernelPanic("APIC ID outside the supported range");

            cpus[apicId].cpuNumber = apicId;
            cpus[apicId].isBP = true;
            cpus[apicId].status = CPU_RUNNING;
        }
    } else {
        apicOps.version = XAPIC;
        apicOps.readApicRegister = ReadXapicRegister;
        apicOps.writeApicRegister = WriteXapicRegister;
        apicOps.checkIndexedStatus = CheckXapicStatus;
        apicOps.writeApicIcr = WriteXapicIcr;
        apicOps.getApicId = XapicGetId;

        if (isBoot) {
            // -- enable the APIC
            Frame_t apicFrame = apicBaseMsr >> 12;
            apicOps.xApicBase = apicBaseMsr & ~(PAGE_SIZE-1);

            WRMSR(IA32_APIC_BASE_MSR, 0
                    | IA32_APIC_BASE_MSR__EN
                    | (apicBaseMsr & ~(PAGE_SIZE-1)));

            MapPage(apicOps.xApicBase, apicFrame, PG_WRT|PG_DEV|PG_KRN);

            apicId = apicOps.readApicRegister(APIC_LOCAL_ID) >> 24;

            if (apicId > MAX_CPU) KernelPanic("APIC ID outside the supported range");

            cpus[apicId].cpuNumber = apicId;
            cpus[apicId].isBP = true;
            cpus[apicId].status = CPU_RUNNING;
        }
    }


    //
    // -- SW enable the Local APIC timer
    //    ------------------------------
    apicOps.writeApicRegister(APIC_ESR, 0);
    NOP();
    apicOps.writeApicRegister(APIC_SIVR, 39 | APIC_SOFTWARE_ENABLE);
    NOP();

    // -- here we initialize the LAPIC to a defined state
    apicOps.writeApicRegister(APIC_DFR, 0xffffffff);

    if (!isX2) apicOps.writeApicRegister(APIC_LDR, apicOps.readApicRegister(APIC_LDR) | (1<<24));

    apicOps.writeApicRegister(APIC_LVT_TIMER, APIC_LVT_MASKED);
    apicOps.writeApicRegister(APIC_LVT_PERF_COUNTING_REG, APIC_LVT_MASKED);
    apicOps.writeApicRegister(APIC_LVT_LINT0, APIC_LVT_MASKED);
    apicOps.writeApicRegister(APIC_LVT_LINT1, APIC_LVT_MASKED);
    apicOps.writeApicRegister(APIC_LVT_ERROR, APIC_LVT_MASKED);
    apicOps.writeApicRegister(APIC_LVT_THERMAL_SENSOR, 0);
    apicOps.writeApicRegister(APIC_TPR, 0);
    apicOps.writeApicRegister(APIC_TIMER_DCR, 0x03);      // divide value is 16
    apicOps.writeApicRegister(APIC_LVT_TIMER, 32);        // timer is vector 32; now unmasked


    // -- enable the PIC timer in one-shot mode
    if (isBoot) {
        OUTB(0x61, (INB(0x61) & 0xfd) | 1);
        OUTB(0x43, 0xb2);

        //
        // -- So, here is the math:
        //    We need to divide the clock by 20 to have a value large enough to get a decent time.
        //    So, we will be measuring 1/20th of a second.
        // -- 1193180 Hz / 20 == 59659 cycles == e90b cycles
        OUTB(0x42, 0x0b);
        INB(0x60);      // short delay
        OUTB(0x42, 0xe9);

        // -- now reset the PIT timer and start counting
        uint8_t tmp = INB(0x61) & 0xfe;
        OUTB(0x61, tmp);
        OUTB(0x61, tmp | 1);

        // -- reset the APIC counter to -1
        apicOps.writeApicRegister(APIC_TIMER_ICR, 0xffffffff);

        while (!(INB(0x61) & 0x20)) {}  // -- busy wait here

        apicOps.writeApicRegister(APIC_LVT_TIMER, APIC_LVT_MASKED);

        // -- remap the 8259 PIC to some obscure interrupts
        OUTB(0x20, 0x11);       // starts the initialization sequence (in cascade mode)
    	OUTB(0xa0, 0x11);
	    OUTB(0x21, 0x40);       // ICW2: Master PIC vector offset
	    OUTB(0xa1, 0x48);       // ICW2: Slave PIC vector offset
	    OUTB(0x21, 4);          // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	    OUTB(0xa1, 2);          // ICW3: tell Slave PIC its cascade identity (0000 0010)

        // -- disable the PIC
        OUTB(0x21, 0xff);
        OUTB(0xa1, 0xff);


        //
        // -- Now we can calculate the cpu frequency, converting back to a full second
        //    ------------------------------------------------------------------------
        uint64_t cpuFreq = (0xffffffff - apicOps.readApicRegister(APIC_TIMER_CCR) * 16 * 20);
        factor = cpuFreq / freq / 16;

        if (((((uint64_t)factor) >> 32) & 0xffffffff) != 0) {
            KernelPanic("PANIC: The APIC frequency factor is too large for the architecture!\n");
        }
    }

    //
    // -- Now, program the Timer
    //    ----------------------
    apicOps.writeApicRegister(APIC_TIMER_ICR, factor);
    apicOps.writeApicRegister(APIC_LVT_TIMER, APIC_LVT_TIMER_PERIODIC | 32);
}



/****************************************************************************************************************//**
*   see arch.h for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
int LapicGetId(void)
{
    return apicOps.getApicId();
}



/****************************************************************************************************************//**
*   @fn                 void LapicEoi(void)
*   @brief              Issue an EOI to the LAPIC
*
*   Issue an End Of Interrupt to the LAPIC
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicEoi(void)
{
    apicOps.writeApicRegister(APIC_EOI, 0);
}



/****************************************************************************************************************//**
*   see arch.h for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicSendInit(int core)
{
    // -- Hi bits are xxxx xxxx 0000 0000 0000 0000 0000 0000
    // -- Lo bits are 0000 0000 0000 xx00 xx0x xxxx 0000 0000
    //                               ++   || | |+-+
    //                               |    || | | |
    //                               |    || | | +--------- delivery mode (101)
    //    Destination Shorthand (00) +    || | +----------- destination mode (0)
    //                                    || +------------- delivery status (1)
    //                                    |+--------------- level (1)
    //                                    +---------------- trigger (1)
    //
    //   or 0000 0000 0000 0000 1101 0101 0000 0000 (0x0000d500)

    uint64_t icr = 0x000000000000d500 | (((uint64_t)core & 0xff) << 56);

    apicOps.writeApicIcr(icr);
}



/****************************************************************************************************************//**
*   see arch.h for documentation
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void LapicSendSipi(int core, Addr_t vector)
{
    // -- Hi bits are xxxx xxxx 0000 0000 0000 0000 0000 0000
    // -- Lo bits are 0000 0000 0000 xx00 xx0x xxxx 0000 0000
    //                               ++   || | |+-+ +-------+
    //                               |    || | | |      +   startup vector (vector >> 12)
    //                               |    || | | +--------- delivery mode (110)
    //    Destination Shorthand (00) +    || | +----------- destination mode (0)
    //                                    || +------------- delivery status (1)
    //                                    |+--------------- level (1)
    //                                    +---------------- trigger (1)
    //
    //   or 0000 0000 0000 0000 1101 0110 0000 0000 (0x0000d600)

    uint64_t icr = 0x000000000000d600 | (((uint64_t)core & 0xff) << 56) | ((vector >> 12) & 0xff);

    apicOps.writeApicIcr(icr);
}


