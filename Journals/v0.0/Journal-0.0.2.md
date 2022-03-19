# CenturyOS

## An Operation System designed to take a Century to complete.

---

OK, here we go again!  I'm starting over.

So, what the hell, Adam???

Here's the deal.  I found something on the web: https://pages.cs.wisc.edu/~remzi/OSTEP/

This "course" from the University of Wisconsin has given me some focus.  Rather than over-engineering the kernel, as I was in the middle of doing in the previous incarnation, I think I can get some basics up and running rather quickly.  And these can be done in some rather small steps.


---

## Version 0.0.2 -- Timer IRQ

This version is relatively simple.  It's sole objective is to get the timer IRQ firing on all cores and get to a point where interrupts can be enabled on all cores.

This is particularly easy since much of the work for getting the timer configured as been completed already and may simply need to be copied (or even just called) for the AP cores.


---

### 2022-Mar-13

I am going to be lazy on this a bit.


---

### 2022-Mar-16

I think I am going to lay some code on this today.  My other projects have taken a big leap forward and I need to consider how the next steps are going to unfold.

So, the timer IRQ....

The first thing I want to do is to establish a CPU Context field in the `cpus` structure to keep track of whether we are in a hardware interrupt or a software system call or exception or what.  I will need to fix-up the existing exception code to set this state on entry and the initialization code to set this state up as well.  So, what types of context do I need?
* INIT
* FENCED
* RUNNING
* FAULTED
* IRQ
* SERVICE
* STOPPED
* ABSENT

So, what are the differences between FAULTED, IRQ, and SERVICE?  Not really anything -- 2 are interrupts of the executing stream of code and the other is an operating system service call.  So, really the FAULTED and IRQ could and should be combined into a single EXCEPTION context.

What about INIT, FENCED, and RUNNING?  Are they not the same?  Well, yes they are.  As far as the CPU is concerned, they are all related to the CPU being active and operating.  So, may as well combine those into just RUNNING.

So the final CPU contexts are:
* RUNNING
* EXCEPTION
* SERVICE
* STOPPED
* ABSENT

I also have a `CPU_STARTING` state or status that I use for CPU Startup.  I really should just extend that status.

Here are the statuses:

```c++
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
    CPU_EXCEPTION = 6,              //!< The CPU is handling an interrupt/exception
    CPU_SERVICE = 7,                //!< The CPU is handling an OS Service Routine
};
```

---

And immediately I can see that `gs` is not being set properly.


---

### 2022-Mar-17

OK, I was able to get the `swapgs` opcode to work.  It is setting the value properly.  However!!!, there are some things that need to be kept track of.  The `swapgs` opcode basically performs a register shadow swap, swapping the `IA32_KERNEL_GS_BASE` and `IA32_GS_BASE` registers for each other.  A subsequent `swapgs` will swap them back.  So, we need to know when we are at ring 0 or ring 3.

The problem is that the CPU structure will not be available until that swap happens if it is not available, so I need to come up with a different method for making this determination.

[This note](https://wiki.osdev.org/SWAPGS#Complications) suggests looking at the contents of the stack pointer for the `cs` register to determine if the system was in ring 3 prior to the call and if it was (or not in ring 0) then perform the swap.

This also suggests that I need to set the `IA32_GS_BASE` MSR to `0` during initialization.

OK, this is settled better now.  Now for the interrupt handlers.  There are several lines to be added to each interrupt handler.  This is better handled through a some macros, which can be part of a prolog/epilog of an interrupt handler.

---

Introduced a bug when switching to the macros.  It's a good thing I tested that as it would have been hidden and an obscure error to find later.

So, the next step will be to write a timer interrupt.


---

### 2022-Mar-18

Well, I was able to get an IRQ target written.  I think it worked.  I see things in the Bochs log which indicate it might have.  But the `DbgPrintf` call did not write anything.

No, my calculation went left:

```
00158241758d[APIC0 ] read from APIC address 0x0000fee00390 = 00000000
00158241783d[APIC0 ] LAPIC write 0x00041893 to register 0x0380
00158241783d[APIC0 ] APIC: Initial Timer Count Register = 268435
```

That will take too long to trigger.


---

### 2022-Mar-19

OK, working theory: there is a problem with the calibration code (I believe) which takes too long to execute and results in a countdown setting too high.  And I did find a bug in my X2APIC code trying to initialize the `APIC_TIMER_CCR` rather than the `APIC_TIMER_ICR`.  Not that this should have created a huge problem (off by 1 tick), but I think it was impacting the calculation enough.

That said, I am getting an interrupt:

```
00158406443d[APIC0 ] acknowledge_int() returning vector 0x20
00158406443i[      ] CPU 0: hardware interrupt 20h
00158406443i[      ] CPU 0: interrupt 20h
00158406443d[CPU0  ] interrupt(): vector = 20, TYPE = 0, EXT = 1
00158406443d[CPU0  ] page walk for address 0xffffc00000107200
00158406443d[CPU0  ] interrupt(long mode): INTERRUPT TO SAME PRIVILEGE
00158406443i[      ] Instruction prepared
00158406443d[CPU0  ] LONG MODE IRET
00158406443d[CPU0  ] LONG MODE INTERRUPT RETURN TO OUTER PRIVILEGE LEVEL or 64 BIT MODE
00158406443i[      ] ----------------------------------------------------------
00158406443i[      ] CPU 0 at 0xffffc00000103cfb: iret    (reg results):
00158406443i[      ] LEN 2	BYTES: 48cf
00158406443i[      ]   RAX: 0x0000000000000004; RBX: 0x0000000000000003; RCX 0x0000000003000000; RDX: 0x0000000000000000
00158406443i[      ]   RSP: 0xffffbffffffffff8; RBP: 0x0000000000200000; RSI 0x0000000000000008; RDI: 0x030000000000d608
00158406443i[      ]   R8 : 0xffffbfffffff0000; R9 : 0x0000000000000000; R10 0x0000000000000000; R11: 0x0000000000000000
00158406443i[      ]   R12: 0x0000000000000000; R13: 0x0000000000000000; R14 0x0000000000000000; R15: 0x0000000000000000
00158406443i[      ]   CS: 0x0008; DS: 0x0028; ES: 0x0028; FS: 0x0000; GS: 0x0098; SS: 0x0010;
00158406443i[      ]   RFLAGS: 0x0000000000200296 (ID vip vif ac vm rf nt IOPL=0 of df IF tf SF zf AF PF cf)
00158406443i[      ] 	BRANCH TARGET ffffc00000103d70 (TAKEN)
00158406443i[      ] MEM ACCESS[0]: 0xffffbfffffffffd8 (linear) 0x000001003fd8 (physical) RD SIZE: 8
00158406443i[      ] MEM ACCESS[1]: 0xffffbfffffffffd0 (linear) 0x000001003fd0 (physical) RD SIZE: 8
00158406443i[      ] MEM ACCESS[2]: 0xffffbfffffffffc8 (linear) 0x000001003fc8 (physical) RD SIZE: 8
00158406443i[      ] MEM ACCESS[3]: 0xffffc00000108008 (linear) 0x000000108008 (physical) RD SIZE: 8
00158406443i[      ] MEM ACCESS[4]: 0xffffbfffffffffe8 (linear) 0x000001003fe8 (physical) RD SIZE: 8
00158406443i[      ] MEM ACCESS[5]: 0xffffbfffffffffe0 (linear) 0x000001003fe0 (physical) RD SIZE: 8
00158406443i[      ] MEM ACCESS[6]: 0xffffc00000108010 (linear) 0x000000108010 (physical) RD SIZE: 8
00158406443i[      ] Instruction prepared
00158406443i[      ] ----------------------------------------------------------
```

I forgot to install the new handler.  So, there is never an ACK.

That worked:

```
Hello, World!
Hello, World from CPU1
Hello, World from CPU2
Hello, World from CPU3
000000000000
```

The list of `0` at the end is a timer interrupt.  Oddly, I get something different from `qemu`, so I need to determine the difference (if I recall correctly, it is a x2APIC so the APIC ID is a full 32-bit):

```
Hello, World!
Hello, World from CPU1
Hello, World from CPU2
Hello, World from CPU3
484848484848
```

Or, is 48 the ASCII decimal representation of `'0'`?  I think it is.

I think there was a change accidentally backed out and recompiled between tests.  Re-applying that change corrected `qemu`.

Now, trying to enable interrupts on the APs results in a triple fault.  So, something is not yet completely right.

That's better:

```
Hello, World!
Hello, World from CPU1
Hello, World from CPU2
2112Hello, World from CPU3
023013201302110233223
```

Well, Bochs is not behaving properly.  It's locking up tight enough I need to kill it from the OS.  Yup, from the logs, this is not right:

```
00158350683i[      ] MEM ACCESS[0]: 0x0000000001013fc0 (linear) 0x000800000000 (physical) RD SIZE: 0
00158350683i[      ] MEM ACCESS[1]: 0x0000000000107208 (linear) 0x000800000000 (physical) RD SIZE: 0

(... snip...)

00158350683i[      ] MEM ACCESS[5208713]: 0x0000000000000000 (linear) 0x000000000000 (physical) RD SIZE: 0
00158350683i[      ] MEM ACCESS[5208714]: 0x0000000000000000 (linear) 0x000000000000 (physical) RD SIZE: 0
00158350683i[      ] MEM ACCESS[5208715]: 0x0000000000000000 (linear) 0x000000000000 (physical) RD SIZE: 0
00158350683i[      ] MEM ACCESS[5208716]: 0x0000000000000000 (linear) 0x000000000000 (physical) RD SIZE: 0
```

5M memory accesses?  With a read size of 0?  On this instruction:

```
00158350683i[      ] CPU 1 at 0xffffc00000103c62: test qword ptr ss:[rsp+8], 0x0000000000000003   (reg results):
```

Let me try a reboot.

---

Well, crap!  That was not the problem.  There is something I am doing which Bochs is not a fan of.

The problem is happening in the instrumentation code, so let me try disabling that.

That better!

```
Hello, World!
Hello, World from CPU1
11Hello, Worl1d from CPU12
2Hello, 1W2orld fro1m2 CPU3
0312031203120312
```

Real hardware works too:

```
### Listening to /dev/ttyUSB0...
Hello, World!
Hello, World from CPU1
011010101010101010
```

So, I think this satisfies all the requirements for this micro-version.




