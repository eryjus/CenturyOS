/****************************************************************************************************************//**
*   @file               acpi.cc
*   @brief              Functions used to parse the ACPI strutures and gather necessary information
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
*   @def                MAKE_SIG
*   @brief              Macro to convert a char[4] into a uint_32
*///-----------------------------------------------------------------------------------------------------------------
#define MAKE_SIG(s)         ((uint32_t)(s[3]<<24)|(uint32_t)(s[2]<<16)|(uint32_t)(s[1]<<8)|s[0])



/****************************************************************************************************************//**
*   @def                RSDP_SIG
*   @brief              This is the signature of the RSDP, expressed as a uint64_t
*///-----------------------------------------------------------------------------------------------------------------
#define RSDP_SIG            ((uint64_t)' '<<56|(uint64_t)'R'<<48|(uint64_t)'T'<<40|(uint64_t)'P'<<32\
                    |(uint64_t)' '<<24|(uint64_t)'D'<<16|(uint64_t)'S'<<8|(uint64_t)'R')




/****************************************************************************************************************//**
*   @enum               MadtIcType
*   @brief              These are the types of Interrupt Controller Structure Types we can have
*///-----------------------------------------------------------------------------------------------------------------
typedef enum {
    MADT_PROCESSOR_LOCAL_APIC = 0,          //!< Processor Local APIC
    MADT_IO_APIC = 1,                       //!< IO APIC
    MADT_INTERRUPT_SOURCE_OVERRIDE = 2,     //!< Interrupt Source Override
    MADT_NMI_SOURCE = 3,                    //!< NMI Source
    MADT_LOCAL_APIC_NMI = 4,                //!< Local APIC NMI
    MADT_LOCAL_APIC_ADDRESS_OVERRIDE = 5,   //!< Local APIC Address Override
    MADT_IO_SAPIC = 6,                      //!< Streamlined IO APIC ?
    MADT_LOCAL_SAPIC = 7,                   //!< Local Streamlined APIC
    MADT_PLATFORM_INTERRUPT_SOURCES = 8,    //!< Platform Interrupt Sources
    MADT_PROCESSOR_LOCAL_X2APIC = 9,        //!< Local Processor X2APIC
    MADT_LOCAL_X2APIC_NMI = 0xa,            //!< X2APIC NMI
    MADT_GIC = 0xb,                         //!< Generic Interrupt Controller
    MADT_GICD = 0xc,                        //!< GIC Distributor
} MadtIcType;



/****************************************************************************************************************//**
*   @typedef            RsdpSig_t
*   @brief              A formalization of the structure of the RSDP signature
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             RsdpSig_t
*   @brief              The signature for locating the RSDP table
*///-----------------------------------------------------------------------------------------------------------------
typedef union RsdpSig_t {
    char signature[8];              //!< The character signature "RSD PTR " \note Notice the trailing space
    uint64_t lSignature;            //!< The same signature characters represented as a 64-bit unsigned integer
} PACKED RsdpSig_t;



/****************************************************************************************************************//**
*   @typedef            Rsdp_t
*   @brief              A formalization of the structure of the RSDP table
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             Rsdp_t
*   @brief              The structure known as the RSDP (Root System Description Pointer)
*///-----------------------------------------------------------------------------------------------------------------
typedef struct Rsdp_t {
    RsdpSig_t sig;                  //!< The RSDP signature \see RsdpSig_t
    uint8_t checksum;               //!< The checksum byte which makes the structure sum to 0 (last byte of the sum)
    char oemId[6];                  //!< The OEM id
    uint8_t revision;               //!< Which ACPI revision
    uint32_t rsdtAddress;           //!< The address of the actual structures (32-bit)
    uint32_t length;                //!< The length of the tables
    uint64_t xsdtAddress;           //!< The address of the actual structures (64-bit)
    uint8_t extendedChecksum;       //!< The checksum byte for the extra fields for 64-bit
    uint8_t reserved[3];            //!< Reserved bytes
} PACKED_16 Rsdp_t;



/****************************************************************************************************************//**
*   @typedef            AcpiSig_t
*   @brief              A formalization of the structure of the ACPI table signature
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             AcpiSig_t
*   @brief              The signature used to determine which ACPI table is being examined
*///-----------------------------------------------------------------------------------------------------------------
typedef union AcpiSig_t {
    char signature[4];              //!< The signature bytes
    uint32_t lSignature;            //!< The same signature characters represented as a 32-bit unsigned integer
} PACKED AcpiSig_t;



/****************************************************************************************************************//**
*   @typedef            AcpiStdHdr_t
*   @brief              A formalization of the structure of the standard ACPI table header
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             AcpiStdHdr_t
*   @brief              The standard ACPI table header found on all tables
*///-----------------------------------------------------------------------------------------------------------------
typedef struct AcpiStdHdr_t {
    AcpiSig_t sig;                  //!< The table signature
    uint32_t length;                //!< The table length
    uint8_t revision;               //!< The version number to which the table conforms
    uint8_t checksum;               //!< The checksum byte which makes the structure sum to 0 (last byte of the sum)
    char oemId[6];                  //!< The OEM id
    uint64_t oemTableId;            //!< The OEM Table ID
    uint32_t oemRevision;           //!< The OEM Revision Level
    uint32_t creatorId;             //!< The creator ID
    uint32_t creatorRevision;       //!< The creator revision Level
} PACKED AcpiStdHdr_t;



/****************************************************************************************************************//**
*   @typedef            Xsdt_t
*   @brief              A formalization of the XSDT table structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             Xsdt_t
*   @brief              The 64-bit XSDT structure used for determining system hardware
*///-----------------------------------------------------------------------------------------------------------------
typedef struct Xsdt_t {
    AcpiStdHdr_t hdr;               //!< The standard ACPI table header
    uint64_t entry[0];              //!< there may be 0 or several entries; length must be checked
} PACKED Xsdt_t;



/****************************************************************************************************************//**
*   @typedef            Rsdt_t
*   @brief              A formalization of the RSDT table structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             Rsdt_t
*   @brief              This is the Root System Description Table (RSDT)
*///-----------------------------------------------------------------------------------------------------------------
typedef struct Rsdt_t {
    AcpiStdHdr_t hdr;               //!< The standard ACPI table header
    uint32_t entry[0];              //!< there may be 0 or several of these; length must be checked
} PACKED Rsdt_t;



/****************************************************************************************************************//**
*   @typedef            MADT_t
*   @brief              A formalization of the MADT table structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             MADT_t
*   @brief              The Multiple APIC Description Table (MADT)
*///-----------------------------------------------------------------------------------------------------------------
typedef struct MADT_t {
    AcpiStdHdr_t hdr;               //!< The standard ACPI table header
    uint32_t localIntCtrlAddr;      //!< Local interrupt controller address
    uint32_t flags;                 //!< Flags
    uint8_t intCtrlStructs[0];      //!< Interrupt Controller Structures
} PACKED MADT_t;



/****************************************************************************************************************//**
*   @typedef            MadtLocalApic_t
*   @brief              A formalization of the Local Processor APIC structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             MadtLocalApic_t
*   @brief              Local Processor APIC structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct MadtLocalApic_t {
    uint8_t type;                   //!< \ref MADT_PROCESSOR_LOCAL_APIC
    uint8_t len;                    //!< Length in bytes
    uint8_t procId;                 //!< Processor ID
    uint8_t apicId;                 //!< APIC ID
    uint32_t flags;                 //!< flags \note 0b00000001 means the processor is enabled
} PACKED MadtLocalApic_t;



/****************************************************************************************************************//**
*   @typedef            MadtIoApic_t
*   @brief              A formalization of the IO APIC structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             MadtIoApic_t
*   @brief              I/O APIC Structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct MadtIoApic_t {
    uint8_t type;                   //!< \ref MADT_IO_APIC
    uint8_t len;                    //!< Length in bytes
    uint8_t apicId;                 //!< APIC ID
    uint8_t reserved;               //!< Reserved byte
    uint32_t ioApicAddr;            //!< IO APIC Address in memory
    uint32_t gsiBase;               //!< Global System Interrupt Number where this APIC starts
} PACKED MadtIoApic_t;



/****************************************************************************************************************//**
*   @typedef            MadtIntSrcOverride_t
*   @brief              A formalization of the Interrupt Source Override Structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             MadtIntSrcOverride_t
*   @brief              Interrupt Source Override Structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct MadtIntSrcOverride_t {
    uint8_t type;                   //!< \ref MADT_INTERRUPT_SOURCE_OVERRIDE
    uint8_t len;                    //!< Length in bytes
    uint8_t bus;                    //!< fixed: 0 = ISA
    uint8_t source;                 //!< IRQ Source
    uint32_t gsInt;                 //!< Global System Interrupt number
    uint32_t flags;                 //!< Flags
} PACKED MadtIntSrcOverride_t;



/****************************************************************************************************************//**
*   @typedef            MadtMNISource_t
*   @brief              A formalization of the NMI Interrupt Source Structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             MadtMNISource_t
*   @brief              NMI Interrupt Source Structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct MadtMNISource_t {
    uint8_t type;                   //!< \ref MADT_NMI_SOURCE
    uint8_t len;                    //!< Length in bytes
    uint16_t flags;                 //!< Flags
    uint32_t gsInt;                 //!< Global System Interrupt
} PACKED MadtMNISource_t;



/****************************************************************************************************************//**
*   @typedef            MadtLocalApicNMI_t
*   @brief              A formalization of the Local APIC NMI Structure
*///-----------------------------------------------------------------------------------------------------------------
/****************************************************************************************************************//**
*   @struct             MadtLocalApicNMI_t
*   @brief              Local APIC NMI Structure
*///-----------------------------------------------------------------------------------------------------------------
typedef struct MadtLocalApicNMI_t {
    uint8_t type;                   //!< \ref MADT_LOCAL_APIC_NMI
    uint8_t len;                    //!< Length in bytes
    uint8_t procId;                 //!< 0xff is all processors
    uint16_t flags;                 //!< Flags
    uint8_t localLINT;              //!< Local APIC interrupt input LINTn to which NMI is connected
} PACKED MadtLocalApicNMI_t;



/****************************************************************************************************************//**
*   @var                rsdp
*   @brief              The location of the RSDP when found
*///-----------------------------------------------------------------------------------------------------------------
Rsdp_t *rsdp = nullptr;



/****************************************************************************************************************//**
*   @def                EBDA
*   @brief              The canonical location of the Extended BIOS Data Area
*///-----------------------------------------------------------------------------------------------------------------
#define EBDA            0x80000



/****************************************************************************************************************//**
*   @def                BIOS
*   @brief              The canonical location of the BIOS
*///-----------------------------------------------------------------------------------------------------------------
#define BIOS 0xe0000



/****************************************************************************************************************//**
*   @def                BIOS_END
*   @brief              The BIOS ending address
*///-----------------------------------------------------------------------------------------------------------------
#define BIOS_END 0xfffff



/****************************************************************************************************************//**
*   @fn                 static bool IsRsdp(Rsdp_t *rsdp)
*   @brief              Validate the RSDP checksum is accurate
*
*   Loop through all the RSDP and add it's values up.  The lowest 8 bits must be 0 for the checksum to be valid.
*
*   @param              rsdp            Pointer to the candidate RSDP
*   @returns            whether the RSDP is valid
*
*   @retval             false           The RSDP is not valid
*   @retval             true            The RSDP is valid
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool IsRsdp(Rsdp_t *rsdp)
{
    if (!rsdp) return false;

    uint32_t cs = 0;
    uint8_t *data = (uint8_t *)rsdp;

    for (uint32_t i = 0; i < 20; i ++) {
        cs += data[i];
    }

    return (cs & 0xff) == 0;
}



/****************************************************************************************************************//**
*   @fn                 static Rsdp_t *AcpiFindRsdp(void)
*   @brief              Locate the Root System Description Table
*
*   Search all the known locations for the RSDP and if found, set its location
*
*   @returns            the location of the RSDP, NULL if not found
*
*   @retval             NULL            when the RSDP table is not found
*   @retval             non-NULL        the address of the RSDP
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
Rsdp_t *AcpiFindRsdp(void)
{
    Addr_t wrk = EBDA & ~0x000f;
    Addr_t end = wrk + 1024;
    Addr_t pg = wrk & ~(PAGE_SIZE - 1);
    Rsdp_t *rsdp;
    Rsdp_t *rv = nullptr;


    MapPage(pg, pg >> 12, PG_KRN);
    if (wrk != 0) {
        while (wrk < end) {
            rsdp = (Rsdp_t *)wrk;

            if (rsdp->sig.lSignature == RSDP_SIG && IsRsdp(rsdp)) {
                rsdp = (Rsdp_t *)wrk;
//                cmn_MmuUnmapPage(pg);
                return rsdp;
            }

            wrk += 16;
        }
    }
//    cmn_MmuUnmapPage(pg);

    wrk = BIOS;
    end = BIOS_END;

    for (pg = wrk; pg < end; pg += PAGE_SIZE) {
        MapPage(pg, pg >> 12, PG_KRN);
    }
    pg = wrk;       // reset pg

    while (wrk < end) {
        rsdp = (Rsdp_t *)wrk;

        if (rsdp->sig.lSignature == RSDP_SIG && IsRsdp(rsdp)) {
            rsdp = (Rsdp_t *)wrk;
            rv = rsdp;
            goto exit;
        }

        wrk += 16;
    }

exit:
    for (pg = wrk; pg < end; pg += PAGE_SIZE) {
//        cmn_MmuUnmapPage(pg);
    }

    return rv;
}



/****************************************************************************************************************//**
*   @fn                 static bool AcpiCheckTable(Addr_t loc, uint32_t sig)
*   @brief              Check the table to see if it is what we expect
*
*   Does the ACPI table at the location provided contain the signature we are looking for?
*
*   @returns            whether the table contains the signature we desire
*
*   @retval             true            This is the desired table
*   @retval             false           This is not the desired table
*
*   @note Memory must be mapped before calling
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool AcpiCheckTable(Addr_t loc, uint32_t sig)
{
    if (loc == 0) return false;

    uint8_t *table = (uint8_t *)loc;
    uint32_t size;
    Addr_t checksum = 0;
//    bool needUnmap = false;
    Addr_t page = loc & ~(PAGE_SIZE - 1);

//    if (!cmn_MmuIsMapped(page)) {
        MapPage(page, page >> 12, PG_KRN);
//        needUnmap = true;
//    }

    if (*((uint32_t *)loc) != sig) {
//        if (needUnmap) cmn_MmuUnmapPage(page);

        return false;
    }

    size = *((uint32_t *)(loc + 4));

    for (uint32_t i = 0; i < size; i ++) {
        Addr_t wrk = (Addr_t)(&table[i]);
//        if (!cmn_MmuIsMapped(wrk)) {
            MapPage(wrk, wrk >> 12, PG_KRN);
//        }
        checksum += table[i];
    }

//    if (needUnmap) cmn_MmuUnmapPage(page);

    return (checksum & 0xff) == 0;
}



/****************************************************************************************************************//**
*   @fn                 void AcpiReadMadt(Addr_t loc)
*   @brief              Read the ACPI MADT Table, and figure out what it means to CenturyOS
*
*   Read the MADT table and determine what the APICs the system has that we might be interested in.
*
*   @param              loc         The location of the MADT table
*
*   @note Memory must be mapped before calling
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void AcpiReadMadt(Addr_t loc)
{
    MADT_t *madt = (MADT_t *)loc;

    uint8_t *wrk = (uint8_t *)(loc + __builtin_offsetof(MADT_t, intCtrlStructs));
    uint8_t *first = wrk;

    while (wrk - first < (long)(madt->hdr.length - __builtin_offsetof(MADT_t,intCtrlStructs))) {
        uint8_t len = wrk[1];

        switch(wrk[0]) {
        case MADT_PROCESSOR_LOCAL_APIC:
            {
                cpuCount ++;
            }

            break;

        case MADT_IO_APIC:
            {
            }

            break;

        case MADT_INTERRUPT_SOURCE_OVERRIDE:
            {
            }

            break;

        case MADT_NMI_SOURCE:
            {
            }

            break;

        case MADT_LOCAL_APIC_NMI:
            {
            }

            break;

        case MADT_LOCAL_APIC_ADDRESS_OVERRIDE:
            DbgPrintf("!!!! MADT IC Table Type MADT_LOCAL_APIC_ADDRESS_OVERRIDE is not supported\n");
            break;

        case MADT_IO_SAPIC:
            DbgPrintf("!!!! MADT IC Table Type MADT_IO_SAPIC is not supported\n");
            break;

        case MADT_LOCAL_SAPIC:
            DbgPrintf("!!!! MADT IC Table Type MADT_LOCAL_SAPIC is not supported\n");
            break;

        case MADT_PLATFORM_INTERRUPT_SOURCES:
            DbgPrintf("!!!! MADT IC Table Type MADT_PLATFORM_INTERRUPT_SOURCES is not supported\n");
            break;

        case MADT_PROCESSOR_LOCAL_X2APIC:
            DbgPrintf("!!!! MADT IC Table Type MADT_PROCESSOR_LOCAL_X2APIC is not supported\n");
            break;

        case MADT_LOCAL_X2APIC_NMI:
            DbgPrintf("!!!! MADT IC Table Type MADT_LOCAL_X2APIC_NMI is not supported\n");
            break;

        case MADT_GIC:
            DbgPrintf("!!!! MADT IC Table Type GIC is not supported\n");
            break;

        case MADT_GICD:
            DbgPrintf("!!!! MADT IC Table Type GICD is not supported\n");
            break;

        default:
            DbgPrintf("!!!! Unknown MADT IC Table Type: %x\n", wrk[0]);
            break;
        }


        wrk += len;
    }
}



/****************************************************************************************************************//**
*   @fn                 static uint32_t AcpiGetTableSig(Addr_t loc)
*   @brief              Get the table signature (and check its valid); return 0 if invalid
*
*   Determine what table the current table is and if it of interest, parse its data.
*
*   @param              loc         The location of the MADT table
*
*   @note This function will map and unmap memory as needed to satisfy access to the memory
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
uint32_t AcpiGetTableSig(Addr_t loc)
{
    if (!loc) return 0;

//    bool needUnmap = false;
    Addr_t page = loc & ~(PAGE_SIZE - 1);

//    if (!cmn_MmuIsMapped(page)) {
        MapPage(page, page >> 12, PG_KRN);
//        needUnmap = true;
//    }

    uint32_t rv = *((uint32_t *)loc);

    if (!AcpiCheckTable(loc, rv)) {
        rv = 0;
        goto exit;
    }

    switch(rv) {
    case MAKE_SIG("APIC"):
        AcpiReadMadt(loc);
        break;

    case MAKE_SIG("BERT"):
        break;

    case MAKE_SIG("BGRT"):
        break;

    case MAKE_SIG("BOOT"):
        break;

    case MAKE_SIG("CPEP"):
        break;

    case MAKE_SIG("CSRT"):
        break;

    case MAKE_SIG("DBG2"):
        break;

    case MAKE_SIG("DBGP"):
        break;

    case MAKE_SIG("DMAR"):
        break;

    case MAKE_SIG("DRTM"):
        break;

    case MAKE_SIG("DSDT"):
        break;

    case MAKE_SIG("ECDT"):
        break;

    case MAKE_SIG("EINJ"):
        break;

    case MAKE_SIG("ERST"):
        break;

    case MAKE_SIG("ETDT"):
        break;

    case MAKE_SIG("FACP"):
        break;

    case MAKE_SIG("FACS"):
        break;

    case MAKE_SIG("FPDT"):
        break;

    case MAKE_SIG("GTDT"):
        break;

    case MAKE_SIG("HEST"):
        break;

    case MAKE_SIG("HPET"):
        break;

    case MAKE_SIG("IBFT"):
        break;

    case MAKE_SIG("IVRS"):
        break;

    case MAKE_SIG("MCFG"):
        break;

    case MAKE_SIG("MCHI"):
        break;

    case MAKE_SIG("MPST"):
        break;

    case MAKE_SIG("MSCT"):
        break;

    case MAKE_SIG("MSDM"):
        break;

    case MAKE_SIG("OEM0"):
    case MAKE_SIG("OEM1"):
    case MAKE_SIG("OEM2"):
    case MAKE_SIG("OEM3"):
    case MAKE_SIG("OEM4"):
    case MAKE_SIG("OEM5"):
    case MAKE_SIG("OEM6"):
    case MAKE_SIG("OEM7"):
    case MAKE_SIG("OEM8"):
    case MAKE_SIG("OEM9"):
    case MAKE_SIG("OEMA"):
    case MAKE_SIG("OEMB"):
    case MAKE_SIG("OEMC"):
    case MAKE_SIG("OEMD"):
    case MAKE_SIG("OEME"):
    case MAKE_SIG("OEMF"):
    case MAKE_SIG("OEMG"):
    case MAKE_SIG("OEMH"):
    case MAKE_SIG("OEMI"):
    case MAKE_SIG("OEMJ"):
    case MAKE_SIG("OEMK"):
    case MAKE_SIG("OEML"):
    case MAKE_SIG("OEMM"):
    case MAKE_SIG("OEMN"):
    case MAKE_SIG("OEMO"):
    case MAKE_SIG("OEMP"):
    case MAKE_SIG("OEMQ"):
    case MAKE_SIG("OEMR"):
    case MAKE_SIG("OEMS"):
    case MAKE_SIG("OEMT"):
    case MAKE_SIG("OEMU"):
    case MAKE_SIG("OEMV"):
    case MAKE_SIG("OEMW"):
    case MAKE_SIG("OEMX"):
    case MAKE_SIG("OEMY"):
    case MAKE_SIG("OEMZ"):
        break;

    case MAKE_SIG("PMTT"):
        break;

    case MAKE_SIG("PSDT"):
        break;

    case MAKE_SIG("RASF"):
        break;

    case MAKE_SIG("RSDT"):
        break;

    case MAKE_SIG("SBST"):
        break;

    case MAKE_SIG("SLIC"):
        break;

    case MAKE_SIG("SLIT"):
        break;

    case MAKE_SIG("SPCR"):
        break;

    case MAKE_SIG("SPMI"):
        break;

    case MAKE_SIG("SRAT"):
        break;

    case MAKE_SIG("SSDT"):
        break;

    case MAKE_SIG("TCPA"):
        break;

    case MAKE_SIG("TPM2"):
        break;

    case MAKE_SIG("UEFI"):
        break;

    case MAKE_SIG("WAET"):
        break;

    case MAKE_SIG("WDAT"):
        break;

    case MAKE_SIG("XSDT"):
        break;

    default:
        {
            char l1 [2] = {0};
            char l2 [2] = {0};
            char l3 [2] = {0};
            char l4 [2] = {0};

            l4[0] = (rv >> 24) & 0xff;
            l3[0] = (rv >> 16) & 0xff;
            l2[0] = (rv >> 8) & 0xff;
            l1[0] = rv & 0xff;

            rv = 0;

            goto exit;
        }
    }

exit:
//    if (needUnmap) cmn_MmuUnmapPage(page);

    return rv;
}



/****************************************************************************************************************//**
*   @fn                 static bool AcpiReadXsdt(Addr_t loc)
*   @brief              Read the XSDT table
*
*   Read the RSDT/XSDT table and determine if the table is really the XSDT table.  Return if the XSDT table was
*   found or not.
*
*   @param              loc         The location of the table
*
*   @returns            whether the location contained the XSDT table
*
*   @retval             true        The table at loc is the XSDT
*   @retval             false       The table at loc is not the XSDT
*
*   @note This function will map and unmap memory as needed to satisfy access to the memory
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool AcpiReadXsdt(Addr_t loc)
{
    if (!loc) return false;
    bool rv = true;
    Addr_t page = loc & ~(PAGE_SIZE - 1);
    Xsdt_t *xsdt = (Xsdt_t *)loc;
    uint32_t entries = 0;

    MapPage(page, page >> 12, PG_KRN);

    if (!AcpiCheckTable(loc, MAKE_SIG("XSDT"))) {
        rv =  false;
        goto exit;
    }

    entries = (xsdt->hdr.length - sizeof(AcpiStdHdr_t)) / sizeof(uint64_t);

    for (uint32_t i = 0; i < entries; i ++) {
        if (xsdt->entry[i]) AcpiGetTableSig(xsdt->entry[i]);
    }

exit:
//    cmn_MmuUnmapPage(page);
    return rv;
}



/****************************************************************************************************************//**
*   @fn                 static bool AcpiReadRsdt(Addr_t loc)
*   @brief              Read the RSDT table
*
*   Read the RSDT/XSDT table and determine if the table is really the RSDT table.  Return if the RSDT table was
*   found or not.
*
*   @param              loc         The location of the table
*
*   @returns            whether the location contained the RSDT table
*
*   @retval             true        The table at loc is the RSDT
*   @retval             false       The table at loc is not the RSDT
*
*   @note This function will map and unmap memory as needed to satisfy access to the memory
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
bool AcpiReadRsdt(Addr_t loc)
{
    if (!loc) return false;
    bool rv = true;
    Addr_t page = loc & ~(PAGE_SIZE - 1);
    Rsdt_t *rsdt = (Rsdt_t *)loc;
    uint32_t entries = 0;

    MapPage(page, page >> 12, PG_KRN);

    if (!AcpiCheckTable(loc, MAKE_SIG("RSDT"))) {
        rv =  false;
        goto exit;
    }

    entries = (rsdt->hdr.length - sizeof(AcpiStdHdr_t)) / sizeof(uint32_t);

    for (uint32_t i = 0; i < entries; i ++) {
        if (rsdt->entry[i]) AcpiGetTableSig(rsdt->entry[i]);
    }

exit:
//    cmn_MmuUnmapPage(page);
    return rv;
}



/********************************************************************************************************************
* -- Documented in arch.h
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void PlatformDiscovery(void)
{
    rsdp = AcpiFindRsdp();
    Addr_t page = ((Addr_t)rsdp) & ~(PAGE_SIZE - 1);

    MapPage(page, page >> 12, PG_KRN);

    if (AcpiCheckTable(rsdp->xsdtAddress, MAKE_SIG("XSDT"))) {
        AcpiReadXsdt(rsdp->xsdtAddress);
    } else if (AcpiCheckTable(rsdp->rsdtAddress, MAKE_SIG("RSDT"))) {
        AcpiReadRsdt(rsdp->rsdtAddress);
    } else {
        cpuCount = 1;
    }

    // -- make sure we do not exceed our capability
    if (cpuCount > MAX_CPU) {
        DbgPrintf("CenturyOS only supports %d CPUs\n", MAX_CPU);
        cpuCount = MAX_CPU;
    }

    if (cpuCount == 0) {
        DbgPrintf("ACPI did not report any discoverable CPUs; assuming 1 CPU\n");
        cpuCount = 1;
    }

    // -- for all CPUs found, if it is not already marked at the BP, mark the CPU as off
    for (int i = 1; i < cpuCount; i ++) {
        if (!cpus[i].isBP) {
            cpus[i].status = CPU_OFF;
        }
    }

//    cmn_MmuUnmapPage(page);
}




