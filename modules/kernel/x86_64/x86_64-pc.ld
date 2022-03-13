/*******************************************************************************************************************/
/*                                                                                                                 */
/*  kernel.ld -- This is the linker script for locating the sections in the target binary                          */
/*                                                                                                                 */
/*        Copyright (c)  2022 -- Adam Clark                                                                        */
/*                                                                                                                 */
/* --------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                 */
/*     Date      Tracker  Version  Pgmr  Description                                                               */
/*  -----------  -------  -------  ----  ------------------------------------------------------------------------  */
/*  2022-Feb-26  Initial   0.0.0   ADCL  Initial version                                                           */
/*                                                                                                                 */
/*******************************************************************************************************************/

OUTPUT_FORMAT(elf64-x86-64)
ENTRY(entry)

PHYS = 0x00100000;
KERNEL = 0xffffc00000000000;

SECTIONS {
    . = PHYS;                           /* Multiboot will place the kernel here in physical memory */

    /*
     * -- Start with the multiboot required code, and all the entry code needed to get the MMU initialized
     *    ------------------------------------------------------------------------------------------------
     */
    .entry : {
        _mbStart = .;
        *(.mboot)                       /* lead with the multiboot header */
        *(.text.entry)                  /* followed by the code to get the mmu initialized */
        *(.data.entry)                  /* followed by the data needed to initialized */
        roStart = .;            LONG(_roStart);
        kernelEnd = .;          LONG(_textEnd);

        *(.bss.entry)                   /* the stack for the first function call */
        . = ALIGN(4096);
        _mbEnd = .;
    }


    /*
     * -- This is where the kernel code will be placed.
     *    ---------------------------------------------
     */
    . += KERNEL;
    .text : AT(ADDR(.text) - KERNEL) {
        _textStart = .;
        *(.init)
        *(.text .text.* .stub .gnu.linkonce.t.*)
        . = ALIGN(4096);
        _roStart = .;
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(4096);

        /*
         * -- This is the trampoline code for the APs
         *    ---------------------------------------
         */
        _smpStart = .;
        *(.smptext)
        *(.smpdata)
        . = ALIGN(4096);
        _smpEnd = .;
        _textEnd = .;
    }


    /*
     * -- We drop in the read/write data here
     *    -----------------------------------
     */
    .data : AT(ADDR(.data) - KERNEL) {
        _dataStart = .;
        *(.data)
        . = ALIGN(4);

        _bssStart = .;
        *(.bss)
        . = ALIGN(4096);
        _bssEnd = .;
        _dataEnd = .;
    }


    /*
     * -- Include debugging information in loader memory
     *    ----------------------------------------------
     */
    .stab : {
        _stabStart = .;
        *(.stab);
        *(.stabstr);
        *(.debug*);
        *(.got*)
        BYTE(0)        /* Force the linker to allocate space for this section */
        . = ALIGN(4096);
        _stabEnd = .;
        _kernelEnd = .;
    }


    .symtab : {
        *(.symtab)
    }

    .strtab : {
        *(.shstrtab)
        *(.strtab)
    }

    /DISCARD/ : {
        *(.comment*)
    }
}
