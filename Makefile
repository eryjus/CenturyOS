#####################################################################################################################
##
##  Makefile -- This is the core makefile for creating the Century OS for any of the supported architectures
##
##        Copyright (c)  2022 -- Adam Clark; See LICENSE.md
##        Licensed under "THE BEER-WARE LICENSE"
##        See License.md for details.
##
## -----------------------------------------------------------------------------------------------------------------
##
##     Date      Tracker  Version  Pgmr  Description
##  -----------  -------  -------  ----  ---------------------------------------------------------------------------
##  2022-Feb-23  Initial   0.0.0   ADCL  Initial version
##
#####################################################################################################################


.SILENT:

#X86_64-LIB = $(shell clang --print-libgcc-file-name)
X86_64-LIB = $(shell x86_64-elf-gcc --print-libgcc-file-name)


##
## -- This is the default rule, to compile everything
##    -----------------------------------------------
.PHONY: all
all: init
	tup


##
## -- This rule will make sure that up is initialized and that we have created all the proper variants
##    ------------------------------------------------------------------------------------------------
.PHONY: init
init: TupRules.inc .doxygen
	if [ ! -f .tup/db ]; then `tup init`; fi;
	tup compiledb
	doxygen .doxygen


##
## -- we need to know the current base folder
##    ---------------------------------------
TupRules.inc: Makefile
	echo WS = `pwd` > $@
	echo X86_64_LDFLAGS = $(dir $(X86_64-LIB)) >> $@
	echo CURRENT_YEAR = `date +%Y` >> $@


## ==================================================================================================================


##
## == These rules make the x86_64-pc target
##    =====================================


##
## -- This is the rule to build the x86_84-pc bootable image
##    ------------------------------------------------------
.PHONY: x86_64-pc
x86_64-pc: init
	tup targets/$@/*
	rm -fR img/x86_64-pc.iso
	rm -fR sysroot/x86_64-pc/*
	mkdir -p sysroot/x86_64-pc img
	cp -fR targets/x86_64-pc/* sysroot/x86_64-pc/
	find sysroot/x86_64-pc -type f -name Tupfile -delete
	find sysroot/x86_64-pc -type f -name '*.map' -delete
	grub2-mkrescue -o img/x86_64-pc.iso sysroot/x86_64-pc


##
## -- Run the x86_64-pc on qemu
##    -------------------------
.PHONY: run-x86_64-pc
run-x86_64-pc: x86_64-pc
	qemu-system-x86_64 -smp sockets=1,cores=4 -m 8192 -serial stdio -cdrom img/x86_64-pc.iso


##
## -- Run the x86_64-pc on bochs
##    --------------------------
.PHONY: bochs-x86_64-pc
bochs-x86_64-pc: x86_64-pc
	bochs-debugger -f .bochsrc64 -q


##
## -- Debug the x86_64-pc on qemu
##    ---------------------------
.PHONY: debug-x86_64-pc
debug-x86_64-pc: x86_64-pc
	qemu-system-x86_64 -smp sockets=1,cores=4 -no-reboot -no-shutdown -m 8192 -serial mon:stdio -cdrom img/x86_64-pc.iso -S -s


##
## -- Write the .iso image to a USB stick (sdb)
##    -----------------------------------------
.PHONY: write-x86_64-pc
write-x86_64-pc: x86_64-pc
	sleep 5
	echo "Writing thumb drive"
	ls -al /dev/sd* | grep /dev/sd[c-z]$
	sudo dd bs=4M if=img/x86_64-pc.iso of=/dev/sdc
	echo "Done"
	sleep 5
	pbl-server /dev/ttyUSB0 .

