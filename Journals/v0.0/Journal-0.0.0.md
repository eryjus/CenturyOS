# CenturyOS

## An Operation System designed to take a Century to complete.

---

OK, here we go again!  I'm starting over.

So, what the hell, Adam???

Here's the deal.  I found something on the web: https://pages.cs.wisc.edu/~remzi/OSTEP/

This "course" from the University of Wisconsin has given me some focus.  Rather than over-engineering the kernel, as I was in the middle of doing in the previous incarnation, I think I can get some basics up and running rather quickly.  And these can be done in some rather small steps.


---

## Version 0.0.0 -- First Steps

The first step to take is to get the kernel to boot and into long mode and print the quintessential *Hello, World!* message.  Getting there will accomplish a few things:
* Implement Multi-Boot requirements
* Establish a local GDT
* Establish a local IDT
* Establish a local TSS
* Initialize Paging
* Jump to Long Mode
* Place kernel in upper memory
* Set up the Serial Port for Output
* Print Hello, World! to the Serial Port

All-in-all, this is quite a bit to accomplish with the first commit of this newest iteration.


---

### 2022-Feb-23

Here we go again!

I have the foundational components copied over.  Let me commit them for the first `master` commit and branch for v0.0.0.

---

OK, I think that will do it for the day.  I will start on the actual code tomorrow.


---

### 2022-Feb-24

OK, the first order of business is to start the entry code.  This code is where the Multi-Boot Loader will enter the kernel.  This code will be assembly language on entry and will switch to long mode on exit.  Everything needed to get into long mode will be provided in this `entry.s` source.  `entry.s` is architecture-specific.


---

### 2022-Feb-25

Today I worked on getting some of the fundamental structures built up and ready to use.  I have much of the foundation ready (I think, anyway) but I do not yet have a binary written.  I am stopping tonight needing to create the linker script.


---

### 2022-Feb-26

Well, I started my debugging with a triple fault.  It would have been truly remarkable had it worked perfectly the first time.  In the meantime, the page for address `0x100000` is not mapped.  Meaning once I get into Long Mode, it cannot find the next instruction.

Some notes:
* `pml4` is located at physical `0x1004000`
* `pdpt` is located at physical `0x1005000`
* `pd` is located at physical `0x1006000`
* `pt` is located at physical `0x1007000`

Address `0x1007100` is being set to `0x100103`, which appears correct.

However, the offset into the `pd` is not correct.  Changing that, I get 1 level deeper into the page tables:

```
<bochs:2> page 0x000000000010024e
PML4: 0x0000000001005103    ps         a pcd pwt S W P
PDPE: 0x0000000001006103    ps         a pcd pwt S W P
 PDE: 0x0000000001007103    ps         a pcd pwt S W P
 PTE: 0x0000000000000000       g pat d a pcd pwt S R p
```

Ahh....   I mathed wrong:

```
<bochs:4> page 0x20000
PML4: 0x0000000001005103    ps         a pcd pwt S W P
PDPE: 0x0000000001006103    ps         a pcd pwt S W P
 PDE: 0x0000000001007103    ps         a pcd pwt S W P
 PTE: 0x0000000000100103       G pat d a pcd pwt S W P
linear page 0x0000000000020000 maps to physical page 0x000000100000
```

With that simple correction, I am in 64-bit Long Mode:

```
00152482522i[CPU0  ] CS.mode = 64 bit
00152482522i[CPU0  ] SS.mode = 64 bit
00152482522i[CPU0  ] EFER   = 0x00000500
00152482522i[CPU0  ] | RAX=0000000000000010  RBX=00000000001026e0
00152482522i[CPU0  ] | RCX=00000000c0000080  RDX=0000000000000000
00152482522i[CPU0  ] | RSP=ffffc00000000000  RBP=0000000000000000
00152482522i[CPU0  ] | RSI=0000000001004000  RDI=000000000100c000
00152482522i[CPU0  ] |  R8=0000000000000000   R9=0000000000000000
00152482522i[CPU0  ] | R10=0000000000000000  R11=0000000000000000
00152482522i[CPU0  ] | R12=0000000000000000  R13=0000000000000000
00152482522i[CPU0  ] | R14=0000000000000000  R15=0000000000000000
00152482522i[CPU0  ] | IOPL=0 ID vip vif ac vm rf nt of df if tf SF zf af PF cf
00152482522i[CPU0  ] | SEG sltr(index|ti|rpl)     base    limit G D
00152482522i[CPU0  ] |  CS:0008( 0001| 0|  0) 00000000 00000fff 1 0
00152482522i[CPU0  ] |  DS:0010( 0002| 0|  0) 00000000 00000fff 1 0
00152482522i[CPU0  ] |  SS:0010( 0002| 0|  0) 00000000 00000fff 1 0
00152482522i[CPU0  ] |  ES:0010( 0002| 0|  0) 00000000 00000fff 1 0
00152482522i[CPU0  ] |  FS:0010( 0002| 0|  0) 00000000 00000fff 1 0
00152482522i[CPU0  ] |  GS:0010( 0002| 0|  0) 00000000 00000fff 1 0
00152482522i[CPU0  ] |  MSR_FS_BASE:0000000000000000
00152482522i[CPU0  ] |  MSR_GS_BASE:0000000000000000
00152482522i[CPU0  ] | RIP=0000000000100293 (0000000000100293)
00152482522i[CPU0  ] | CR0=0xe0000011 CR2=0x0000000000000000
00152482522i[CPU0  ] | CR3=0x0000000001004000 CR4=0x000000a0
```

Now to verify the stack mapping....

```S
        call        testStack

        jmp         kInit

testStack:
        ret

```

Nope. Triple fault.  The stack is not properly mapped.  But it almost is.

```
<bochs:2> page 0xffffbffffffffff8
PML4: 0x0000000001008103    ps         a pcd pwt S W P
PDPE: 0x0000000001009103    ps         a pcd pwt S W P
 PDE: 0x000000000100a103    ps         a pcd pwt S W P
 PTE: 0x0000000000000000       g pat d a pcd pwt S R p
physical address not available for linear 0xffffbffffffff000
```

Turns out I was adding individual bytes to the PT locations, not 8-byte offsets.  A quick change corrected that as well.

Now, the next step is to get the code into upper memory (`0xc00000000000` -- 48 bits long; the top 16 bits are dropped).

Hmmm....  Triple fault again.  Wasn't really expecting that.  OK, the address of my upper memory code is wrong.  That was a quick change to correct.  Testing again.  That worked.

---

Now, back to the goals of these first steps.  I need to complete quite a bit more to call this complete.  Let's start with a proper GDT.  I need to replace the existing one with one properly established in the kernel address space.

---

The GDT changes are complete.

I was also able to get the serial output set up, along with a relatively robust version of `DbgPrintf()`.  I am able to output the *Hello, World!* message which was one of the goals of this version.  However, there are a few things which are not yet established.

1. The TSS is not set up (will need/want a `cpus` structure for this and patch the GDT)
2. The IDT is not set up (need to map every possible vector so that anything that occurs without warning can be trapped or responded to)
3. The facilities to map a page do not yet exist

All-in-all, quite a bit has been accomplished, but there is still quite a bit more to do.


---

### 2022-Feb-27

I have the 32 possible hardware interrupts written (albeit trivially) and a general-purpose IRQ and software-generated interrupt handler written (again, trivially).  These are all documented in code properly.

Now, to hook the IDT (which will need to be moved to upper memory anyway and probably should be removed from `entry.s`.

I was reading the Intel SDM Volume 3, Chapter 6.12.1.3 and the handling of the `IF` flag on trap or interrupt gate handler.  It states in part:

> The only difference between an interrupt gate and a trap gate is the way the processor handles the IF flag in the EFLAGS register. When accessing an exception- or interrupt-handling procedure through an interrupt gate, the processor clears the IF flag to prevent other interrupts from interfering with the current interrupt handler. A subsequent IRET instruction restores the IF flag to its value in the saved contents of the EFLAGS register on the stack.  Accessing a handler procedure through a trap gate does not affect the IF flag.

This has me thinking about whether I need Interrupt Gates or Trap Gates for any given handler.

For now, I am going to use the Interrupt Gates.  I was able to get the IDT set up properly and tested with a `#PF`.

---

Next steps here will be to create a function to map a page in memory.  This will be a little trickier, as I do want this to be a little more generic in the OS which will have to call an architecture-specific version to complete the mapping.

---

It dawns on me that I actually will need Physical Memory Manager (PMM) to complete this version.  Hmmmm.....

---

OK, I am going to need some method of allocating a new frame for the VMM to handle the paging tables.  At this point, I only have a rudimentary incremental allocator.  At some point, I will need to allocate from a proper PMM which can accept released frames as well.

At this point, I am thinking to add this into the kernel to handle this abstraction layer (rather than its own process).  With that, I should be able to add the memory map from GRUB into the PMM.  I should be able to have a stack of frame blocks which can be allocated.  If I do this right, I should be able to allocate space for this and have it mapped properly into the VMM with enough space to get by until the `#PF` handler can get fleshed out.

But I ramble....

I need to revisit this tomorrow.  I need to think on this a bit.


---

### 2022-Feb-28

OK, I went back over my notes.  I have a later step set aside for handling the PMM creation.  So, for now, I will be handling the physical memory allocation using the early allocator I created for the loader.  What I need to do, though, is make sure it is properly wrapped so I can just add it in easily when the PMM has been initialized.

---

I have the page mapping functions worked out.  The final thing to do here is to create the TSS.


---

### 2022-Mar-01

Today, I plan to get the majority of the remainder of this version wrapped up.  This will include the start of the `cpu` structure wherein the TSS will be located.  It will include getting CPU0's `gs` and `tss` installed into the GDT properly.

---

OK, I got a clean boot.  I think this is reasonable.  I need to go through each source code file and make sure there are no glaring issues and then I think I can wrap up this version.

---

This version is ready to be called complete.  I am going to commit this version now and then I should be able to start the next version in the next few days.




