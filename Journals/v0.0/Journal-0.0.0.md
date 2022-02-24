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
