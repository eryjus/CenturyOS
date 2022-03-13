# CenturyOS

## An Operation System designed to take a Century to complete.

---

OK, here we go again!  I'm starting over.

So, what the hell, Adam???

Here's the deal.  I found something on the web: https://pages.cs.wisc.edu/~remzi/OSTEP/

This "course" from the University of Wisconsin has given me some focus.  Rather than over-engineering the kernel, as I was in the middle of doing in the previous incarnation, I think I can get some basics up and running rather quickly.  And these can be done in some rather small steps.


---

## Version 0.0.1 -- SMP Startup

This version is also quite ambitious with many things to accomplish:

* Go back and make sure the kernel mappings are correct (all the different permissions bits -- [#578](http://eryjus.ddns.net:3000/issues/578))
* Go back and properly align the functions and variables to the correct sections ([#579](http://eryjus.ddns.net:3000/issues/579))
* Determine the number of CPUs on the system
* Establish the timer interrupt and its handler
* Develop the x86_64 trampoline code and properly relocate it
* Add to the `Cpu_t` structure to track all the necessary stuff
* Complete the TSS setup as appropriate
* Print a "Hello, world!" message from each core
* Fence each core until released


---

### 2022-Mar-02

It's hot today and I have a bit of a heat headache going....  I really do not want to get too deep into heavy research.  I even put aside my other project (Breadboard Computer) so I did not have to research too much.

Looking at the TODO list, I think I am going to determine what it takes to find the HPET.

---

Hmmm....  [This](https://en.wikipedia.org/wiki/High_Precision_Event_Timer#Use_and_compatibility) implies I should probably stick with the x2APIC timer.

I want to think on this a bit, but now that I read more into the HPET it sounds like the HPET will be more problems than it is worth.

So, there are 2 versions of the APIC I really need to consider: the XAPIC (eXtended APIC) and the x2APIC (eXtended XAPIC).  One uses MSRs to interact with the hardware whereas the other uses MMIO addresses to interact.

I recall that the code is largely the same, with the exception really being some of the initialization and how to read/write to the registers.

Before I get into that, I want to make sure I have some helper macros set up to drop functions into the correct sections.

The `.rodata` section should be interesting to manage.  Most constants should be baked right into the code.  The strings and other compound structures are really the only ones I need to worry about.  Strings in particular are ones which will bite me as most data structures are not going to be read only.  Most.

[See here](https://godbolt.org/z/c6nc8eM8W) for a sample.

So, I can start properly decorating the functions and data that live in the kernel address space.  Later on in this version, I will need to add others for the trap landing zone.

Now, for the XAPIC and x2APIC....  First thing will be to determine if I have one.  Intel reports:

> On processors supporting x2APIC architecture (indicated by CPUID.01H:ECX[21] = 1), the local APIC supports operation both in xAPIC mode and (if enabled by software) in x2APIC mode. x2APIC mode provides extended processor addressability (see Section 10.12).

In addition, the following is relevant:

```C++
    // -- First check for xAPIC support
    CPUID(1, &eax, &ebx, &ecx, &edx);
    if (edx & CPUID_FEAT_EDX_APIC) {
```

So, I can use CPUID to determine how much APIC support I have.


---

### 2022-Mar-04

Since the CPUID instruction has so much associated with it, I started the day by moving all that mess into its own header file.  I may end up doing the same with the Model Specific Registers (MSRs) and maybe even for all the other opcodes.


---

### 2022-Mar-05

I have the XAPIC and x2APIC code largely written.  Theoretically, if I enable interrupts, I should get a timer to fire after calling the initialization code.  I do not yet have any kind of a handler written yet and the handler would be the trivial `iretq` version.  But, I should be able to get it to run with interrupts enabled without getting any form of fault.

Well, I am getting a `#PF`.  Let's see what that is all about.

I was not setting the base address properly for the xapic.  So, that was a quick fix.  I believe qemu offers an x2apic, so let's try there....

Again, that is working.  Well, to be clear, I am not getting any form of fault or CPU exception.  I still do not have any form of an interrupt handler that will actually service the IRQ.

Ok, I have some sort of a timer interrupt (maybe).  But that was not part of the goals for this version.

What I really need to get into is parsing the ACPI tables to determine how many CPUs I have on the system.  Part of the problem I am going to have with the ACPI tables will be the location and whether to leave them mapped in the end.

Hmmm....  MultiBoot provides a copy of the ACPI tables.  However, I am not sure I want to have to count on that.

I think I am going to have to do some debugging output.  What I am wondering is if I could/should do this discovery before getting into long mode.  It would simplify the mapping and un-mapping of virtual memory, but would also mean doing all this in 32-bit assembly.

Let me start with determining the location of these tables.

Bochs reports: `The location of the Rsdp table is at 00000000000fa080`
Qemu reports: `The location of the Rsdp table is at 00000000000f5ac0`
VirtualBox offers a triple fault, and this appears to happen before the "Hello, world!" greeting.

So, here is what I found in the logs:

```
00:00:07.349417 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
00:00:07.349418 !!
00:00:07.349419 !!         VCPU0: Guru Meditation 1155 (VINF_EM_TRIPLE_FAULT)
00:00:07.349426 !!
00:00:07.349438 !! Skipping ring-0 registers and stack, rcErr=VINF_EM_TRIPLE_FAULT
00:00:07.349444 !!
00:00:07.349444 !! {mappings, <NULL>}
00:00:07.349445 !!
00:00:07.349452 !!
00:00:07.349453 !! {hma, <NULL>}
00:00:07.349453 !!
```

A little farther down, I have the CPU state:

```
00:00:07.349510 Guest CPUM (VCPU 0) state:
00:00:07.349513 rax=00000000fee00000 rbx=0000000000000020 rcx=0000000000000020 rdx=0000000000000001
00:00:07.349516 rsi=0000000000000010 rdi=0000000000000020 r8 =00000000fee00000 r9 =0000000000000000
00:00:07.349518 r10=0000000000000000 r11=0000000000000000 r12=0000000000000000 r13=0000000000000000
00:00:07.349520 r14=0000000056da2203 r15=00000000fee00900
00:00:07.349521 rip=ffffc00000102a76 rsp=ffffbfffffffffb8 rbp=0000000000000000 iopl=0      rf nv up di pl zr na pe nc
00:00:07.349524 cs={0008 base=0000000000000000 limit=00000fff flags=0000a09b}
00:00:07.349526 ds={0010 base=0000000000000000 limit=00000fff flags=0000a093}
00:00:07.349527 es={0010 base=0000000000000000 limit=00000fff flags=0000a093}
00:00:07.349528 fs={0010 base=0000000000000000 limit=00000fff flags=0000a093}
00:00:07.349529 gs={0010 base=ffffc00000105128 limit=00000fff flags=0000a093}
00:00:07.349531 ss={0010 base=0000000000000000 limit=00000fff flags=0000a093}
00:00:07.349531 cr0=0000000080000011 cr2=0000000000000000 cr3=0000000001004000 cr4=00000000000000a0
00:00:07.349534 dr0=0000000000000000 dr1=0000000000000000 dr2=0000000000000000 dr3=0000000000000000
00:00:07.349535 dr4=0000000000000000 dr5=0000000000000000 dr6=00000000ffff0ff0 dr7=0000000000000400
00:00:07.349537 gdtr=ffffc00000105000:00f7  idtr=ffffc00000104000:0fff  eflags=00010002
00:00:07.349540 ldtr={0000 base=00000000 limit=0000ffff flags=00000082}
00:00:07.349541 tr  ={00a0 base=ffffc00000105136 limit=0000006f flags=0000008b}
00:00:07.349543 SysEnter={cs=0000 eip=0000000000000000 esp=0000000000000000}
00:00:07.349563 xcr=0000000000000001 xcr1=0000000000000000 xss=0000000000000000 (fXStateMask=0000000000000000)
00:00:07.349566 FCW=037f FSW=0000 FTW=0000 FOP=0000 MXCSR=00001f80 MXCSR_MASK=0000ffff
00:00:07.349568 FPUIP=00000000 CS=0000 Rsrvd1=0000  FPUDP=00000000 DS=0000 Rsvrd2=0000
00:00:07.349572 ST(0)=FPR0={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349577 ST(1)=FPR1={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349581 ST(2)=FPR2={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349584 ST(3)=FPR3={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349588 ST(4)=FPR4={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349591 ST(5)=FPR5={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349594 ST(6)=FPR6={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349598 ST(7)=FPR7={0000'00000000'00000000} t0 +0.0000000000000000000000 * 2 ^ -16383 (*)
00:00:07.349602 XMM0 =00000000'00000000'00000000'00000000  XMM1 =00000000'00000000'00000000'00000000
00:00:07.349607 XMM2 =00000000'00000000'00000000'00000000  XMM3 =00000000'00000000'00000000'00000000
00:00:07.349612 XMM4 =00000000'00000000'00000000'00000000  XMM5 =00000000'00000000'00000000'00000000
00:00:07.349617 XMM6 =00000000'00000000'00000000'00000000  XMM7 =00000000'00000000'00000000'00000000
00:00:07.349621 XMM8 =00000000'00000000'00000000'00000000  XMM9 =00000000'00000000'00000000'00000000
00:00:07.349626 XMM10=00000000'00000000'00000000'00000000  XMM11=00000000'00000000'00000000'00000000
00:00:07.349631 XMM12=00000000'00000000'00000000'00000000  XMM13=00000000'00000000'00000000'00000000
00:00:07.349642 XMM14=00000000'00000000'00000000'00000000  XMM15=00000000'00000000'00000000'00000000
00:00:07.349647 EFER         =0000000000000500
00:00:07.349648 PAT          =0007040600070406
00:00:07.349648 STAR         =0000000000000000
00:00:07.349649 CSTAR        =0000000000000000
00:00:07.349649 LSTAR        =0000000000000000
00:00:07.349650 SFMASK       =0000000000000000
00:00:07.349650 KERNELGSBASE =0000000000000000
00:00:07.349690 Display::i_handleDisplayResize: uScreenId=0 pvVRAM=00007fbe5ab99000 w=1024 h=768 bpp=16 cbLine=0x800 flags=0x0 origin=0,0
```

Which led me to the `rip` of the fault: `rip=ffffc00000102a76`

This took me to the disassembly of the kernel:

```
ffffc00000102a50 <ReadXapicRegister>:
ffffc00000102a50:       53                      push   %rbx
ffffc00000102a51:       89 fb                   mov    %edi,%ebx
ffffc00000102a53:       48 b8 f0 26 10 00 00    movabs $0xffffc000001026f0,%rax
ffffc00000102a5a:       c0 ff ff
ffffc00000102a5d:       ff d0                   callq  *%rax
ffffc00000102a5f:       89 c1                   mov    %eax,%ecx
ffffc00000102a61:       31 c0                   xor    %eax,%eax
ffffc00000102a63:       84 c9                   test   %cl,%cl
ffffc00000102a65:       74 12                   je     ffffc00000102a79 <ReadXapicRegister+0x29>
ffffc00000102a67:       48 b8 40 53 10 00 00    movabs $0xffffc00000105340,%rax
ffffc00000102a6e:       c0 ff ff
ffffc00000102a71:       48 8b 00                mov    (%rax),%rax
ffffc00000102a74:       89 d9                   mov    %ebx,%ecx
ffffc00000102a76:       8b 04 08                mov    (%rax,%rcx,1),%eax
ffffc00000102a79:       5b                      pop    %rbx
ffffc00000102a7a:       c3                      retq
ffffc00000102a7b:       0f 1f 44 00 00          nopl   0x0(%rax,%rax,1)
```

Where I am making an attempt to read an APIC register and presumably getting a `#PF` for my trouble.  The faulting instruction: `mov    (%rax,%rcx,1),%eax`

So, the contents of those registers:  `rax=00000000fee00000 rcx=0000000000000020`, resulting in reading from `(00000000fee00020)`.

The register I am trying to read is `APIC_LOCAL_ID = 0x20, //!< Local APIC ID Register`.

The page appears to be mapped:

```
VBoxDbg> dptg 0x00000000fee00020
%%000000000100d000 (base %00000000fee00000 / index 0x0):
000 %00000000fee00000: 00000000fee0033f 4kb phys=00000000fee00000 p  w u a  nd avl=01 g pwt pcd
```

Trying to dump the page table and contents from the vbox debugger:

```
VBoxDbg> dpta 0x00000000fee00020
%00000000fee00020:
error: VERR_PGM_PHYS_PAGE_RESERVED:  Reading PTE memory at %00000000fee00020.
```

So, this feels like 1 of 2 problems to me:
1. (Unlikely) The actual memory being read is not available to the vbox application.
2. (Much more likely) I have a problem with how the page is being mapped and vbox more closely resembles real hardware.

But I boot on real hardware just fine.

In fact, real hardware reports the location of the ACPI tables to be `The location of the Rsdp table is at 00000000000f68c0`

As much as I hate to admit it, I do not think I will be able to use vbox at the moment.  It will take longer to figure out the root problem than I want to invest in the problem.

That said, what I did figure out was that the ACPI tables are mapped < 1MB in all situations.  Or at least the start of them are.

I got that figured out as I was not checking to make sure I got a good address before trying to read it.

I also think I have the CPU count figured out.

I am going to sit on this tonight and revisit code cleanup in the morning.


---

### 2022-Mar-06

OK, I need to take care of a couple of things:
1. Find a place to store the real CPU count.
2. Make sure I am dealing with CPU counts within limits.
3. If I am unable to find the ACPI structures, assume 1 CPU (and report this fact).

---

All 3 of these tasks are complete.


---

### 2022-Mar-07

OK, I cheated and copied the AP entry code from an old version.  I will need to go back through it to make sure it is exactly what I want.  The concern at this point is that the AP entry code is in a `.smptext` section and the linker is dropping that section.  Without any warnings!

So, I actually found the `.smptext` section, but I also found the `--orphan-handling=error` option and added that to the linker script.  I want to know is I did not do something right when I link.  I'm not doing something right when I link:

```
ld.lld: error: ../../../obj/kernel/x86_64-pc/acpi.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/arch-init.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/arch-mmu.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/cpu.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/dbg-printf.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/idt.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/kInit.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/kernel-panic.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/lapic.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/mmu.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/pc-serial.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/pmm.o:(.comment) is being placed in '.comment'
ld.lld: error: ../../../obj/kernel/x86_64-pc/serial.o:(.comment) is being placed in '.comment'
ld.lld: error: <internal>:(.comment) is being placed in '.comment'
ld.lld: error: <internal>:(.symtab) is being placed in '.symtab'
ld.lld: error: <internal>:(.shstrtab) is being placed in '.shstrtab'
ld.lld: error: <internal>:(.strtab) is being placed in '.strtab'
```

I need to clean that all up.

I think I want to keep all of these for debugging.

Well, I did end up explicitly discarding the `.comment` section.  It really had nothing of value -- just the compiler version and the like.

The kernel still boots, so that is good.

I also notice a difference between `entry.s` and `entryAp.s`:

```s
NewGdt:
    mov         rax,0x10
    mov         ss,ax

    mov         rax,0x28
    mov         ds,ax
    mov         es,ax
```

The use of the `0x28` section number needs to be reconsidered.  I did change it and it still boots.  Yay me!

OK, so now I need to write the code to populate the AP Startup Section.  That will be a tomorrow task (after I get through the breadboard computer tasks, which I am prioritizing until I get the current module complete).


---

### 2022-Mar-08

OK, I think I would rather work on this project today.  I really do not want to work my fingers that hard today.

OK, I have the code to copy and set up the trampoline code.  Now I just need to be able to allocate and set up a stack so it can be used to get into long mode on the AP.  Actually, the stack was easy to implement.

So, now I need to handle the IPIs to actually start the CPU.


---

### 2022-Mar-10

After remembering (figuring out) that the APIC ID is different between the XAPIC and x2APIC, I added specific functions for each and I have eliminated the triple fault.  So all CPUs are starting.   Yay!

That is not to say that I am ready to commit.  I still have several things to accomplish.  Specifically:
* Need to establish the TSS
* Sort out sections for each function/data element
* Complete the documentation
* Clear the bss section on entry
* Clean up the kernel MMU mappings properly
* Set the `gs` register for all cores
* Init the LAPIC for each core

---

So, the TSS and `gs` are complete.  I also have the LAPIC getting initialized.

This, then, leaves the cleanup tasks.

Tomorrow or this weekend.


---

### 2022-Mar-11

So, what is left?
* Sections audit
* Document all elements
* Clear the BSS section
* Clean up the MMU mappings

Starting with the MMU, I need to boot into Bochs so I can easily investigate each mapping.  First, lets determine what is what:

```
Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .entry            PROGBITS         0000000000100000  00001000
       0000000000001000  0000000000000000   A       0     0     8
  [ 2] .text             PROGBITS         ffffc00000101000  00002000
       0000000000005000  0000000000000000 AXMS       0     0     16
  [ 3] .data             PROGBITS         ffffc00000106000  00007000
       0000000000002000  0000000000000000  WA       0     0     4096
  [ 4] .stab             PROGBITS         ffffc00000108000  00009000
       000000000000c000  0000000000000000 WAMS       0     0     8
  [ 5] .symtab           SYMTAB           0000000000000000  00015000
       00000000000013b0  0000000000000018           6   104     8
  [ 6] .strtab           STRTAB           0000000000000000  000163b0
       000000000000083e  0000000000000000           0     0     1
```

OK, entry is not something to be concerned with as it is throw-away mappings.

The `.text` section needs to be mapped read-only (not writable).

The `.data` section needs to be mapped read/write.

The `.stab`, `.symtab`, and `.strtab` sections are not used at this time so no need to be concerned about those yet.

So, the page mappings look like this (G, PCD and PWT are not enabled at this point, so they are ignored):

| Address        | Wrt | Usr | Xd  |
|:--------------:|:---:|:---:|:---:|
| 0000 0010 0000 |  1  |  0  |  0  |
| c000 0010 1000 |  0  |  0  |  0  |
| c000 0010 2000 |  0  |  0  |  0  |
| c000 0010 3000 |  0  |  0  |  0  |
| c000 0010 4000 |  0  |  0  |  0  |
| c000 0010 5000 |  0  |  0  |  0  |
| c000 0010 6000 |  1  |  0  |  1  |
| c000 0010 7000 |  1  |  0  |  1  |

So, now I need to reconsider the `.rodata` section which should also probably have the `xd` flag set.  For that to really be of any benefit, I will need to align the `.rodata` section to the 4K boundary.  Doing that is not really that big of a deal, so I will set that up.  Also, the `.smptext` section is considered `.rodata` while it is in kernel space, so that can be properly decorated as well.

So, given the above expectations, what does Bochs report that I did?

* 0000 0010 0000 -- Matches except that the Global flag is set; xd unknown
* c000 0010 1000 -- Writable when it should not be; Global is set; xd unknown
* c000 0010 2000  -- Writable when it should not be; Global is set; xd unknown
* c000 0010 3000  -- Writable when it should not be; Global is set; xd unknown
* c000 0010 4000  -- Writable when it should not be; Global is set; xd unknown
* c000 0010 5000  -- Writable when it should not be; Global is set; xd unknown
* c000 0010 6000 -- Global is set; xd unknown
* c000 0010 7000 -- Global is set; xd unknown

Additionally, several pages past the end of the kernel are mapped as well, which need to be cleaned up.

So, the question is what do I do with it?  Do I clean it up in the entry or do I clean it up before starting the other cores?


---

### 2022-Mar-12

I am still torn on this how to handle the MMU mappings.  Part of me is telling myself to do it right from the start and part of me is saying that this is all very arch-specific (which `entry.s` is very arch-specific) and to handle it accordingly.

And part of me is simply lazy and does not want to invest the effort into assembly.

So, assembly language it is prior to getting into long mode.

I think the think to do here is to add a function to set the proper flags in a register and then return the results.  I can use whatever registers I want as long as I maintain a single register interface for the flag results and save everything else I touch.  It will break the ABI, but this function will not be called from outside this source module.

It looks like `edi` and `ebx` are not used in this section at this point.  `ebp` is also available.

It looks like `NewFrame` clobbers edi, so that should be avoided if possible.

`ebp` and `ebx` it is!

---

OK, I have that worked out.  The pages are mapped with better permissions and there is no residual cleanup from it.

I think the next thing will be to clear the bss section.

Tomorrow.


---

### 2022-Mar-13

OK, the `.bss` section was cleared.

What is left is a slow-walk through all the code to make sure everything shows up on the proper section and that every function is documented properly.

Now, I get to the interrupt handlers whereby I really do not want them in the kernel but rather in a landing zone, mapped differently.

---

OK, I think I have this done:

```
Hello, World!
Hello, World from CPU1
Hello, World from CPU2
Hello, World from CPU3
```



