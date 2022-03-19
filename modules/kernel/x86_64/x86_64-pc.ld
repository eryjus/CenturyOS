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
    /* Stabs debugging sections.  */
    .stab          0        : { *(.stab) }
    .stabstr       0        : { *(.stabstr) }
    .stab.excl     0        : { *(.stab.excl) }
    .stab.exclstr  0        : { *(.stab.exclstr) }
    .stab.index    0        : { *(.stab.index) }
    .stab.indexstr 0        : { *(.stab.indexstr) }
    .comment       0        : { *(.comment) }
    .gnu.build.attributes   : { *(.gnu.build.attributes .gnu.build.attributes.*) }
    .debug          0       : { *(.debug) }
    .line           0       : { *(.line) }
    .debug_srcinfo  0       : { *(.debug_srcinfo) }
    .debug_sfnames  0       : { *(.debug_sfnames) }
    .debug_aranges  0       : { *(.debug_aranges) }
    .debug_pubnames 0       : { *(.debug_pubnames) }
    .debug_info     0       : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0       : { *(.debug_abbrev) }
    .debug_line     0       : { *(.debug_line .debug_line.* .debug_line_end) }
    .debug_frame    0       : { *(.debug_frame) }
    .debug_str      0       : { *(.debug_str) }
    .debug_loc      0       : { *(.debug_loc) }
    .debug_macinfo  0       : { *(.debug_macinfo) }
    .debug_weaknames 0      : { *(.debug_weaknames) }
    .debug_funcnames 0      : { *(.debug_funcnames) }
    .debug_typenames 0      : { *(.debug_typenames) }
    .debug_varnames  0      : { *(.debug_varnames) }
    .debug_pubtypes 0       : { *(.debug_pubtypes) }
    .debug_ranges   0       : { *(.debug_ranges) }
    .debug_macro    0       : { *(.debug_macro) }
    .debug_addr     0       : { *(.debug_addr) }
    .gnu.attributes 0       : { KEEP (*(.gnu.attributes)) }

    .strtab         0       : { KEEP (*(.strtab)) }
    .shstrtab       0       : { KEEP (*(.shstrtab)) }
    .symtab         0       : { KEEP (*(.symtab)) }

    /DISCARD/ : {
        *(.comment*)
    }
}
