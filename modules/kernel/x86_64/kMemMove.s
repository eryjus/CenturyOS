;;===================================================================================================================
;;
;;  kMemMove.s -- Copy a block of memory to the specified location
;;
;;        Copyright (c)  2022 -- Adam Clark
;;        Licensed under "THE BEER-WARE LICENSE"
;;        See License.md for details.
;;
;;  Copy a block of memory to the specified location
;;
;;  On entry, the stack has the following structure:
;;  +-----------+------------------------------------+
;;  |   reg     |  description of the contents       |
;;  +-----------+-----------+------------------------------------+
;;  |   rdx     |  Number of bytes to set            |
;;  +-----------+------------------------------------+
;;  |   rsi     |  The byte to set in the memory     |
;;  +-----------+------------------------------------+
;;  |   rdi     |  The memory location to set        |
;;  +-----------+------------------------------------+
;;  |   rsp     |  Return RIP                        |
;;  +-----------+------------------------------------+
;;
;;  Prototype:
;;  void kMemMove(void *tgt, void *src, size_t cnt);
;;
;; ------------------------------------------------------------------------------------------------------------------
;;
;;     Date      Tracker  Version  Pgmr  Description
;;  -----------  -------  -------  ----  ---------------------------------------------------------------------------
;;  2022-Mar-08  Initial  v0.0.1   ADCL  Initial version
;;
;;===================================================================================================================


;;
;; -- Expose labels to fucntions that the linker can pick up
;;    ------------------------------------------------------
    global  kMemMove


;;
;; -- This is the beginning of the code segment for this file
;;    -------------------------------------------------------
    section .text
    bits    64


;;
;; -- Copy a block of memory to a new location
;;    ----------------------------------------
kMemMove:
    mov     rcx,rdx                     ;; get the number of bytes to set
    xor     rax,rax                     ;; clear rax
    cld                                 ;; make sure we are incrementing
    rep     movsb                                               ;; copy the bytes
    ret
