;;===================================================================================================================
;;
;;  @file               kStrLen.s
;;  @brief              String Length implementation
;;  @author             Adam Clark (hobbyos@eryjus.com)
;;  @date               2022-Feb-26
;;  @since              v0.0.0
;;
;;  @copyright          Copyright (c)  2022 -- Adam Clark\n
;;                      Licensed under "THE BEER-WARE LICENSE"\n
;;                      See \ref LICENSE.md for details.
;;
;;  Calculate the length of a string, returning the result in rax
;;
;; ------------------------------------------------------------------------------------------------------------------
;;
;;     Date      Tracker  Version  Pgmr  Description
;;  -----------  -------  -------  ----  ---------------------------------------------------------------------------
;;  2022-Feb-26  Initial  v0.0.0   ADCL  Initial version
;;
;;===================================================================================================================


;;
;; -- Expose labels to fucntions that the linker can pick up
;;    ------------------------------------------------------
    global      kStrLen


;;
;; -- This is the beginning of the code segment for this file
;;    -------------------------------------------------------
    section     .text
    bits        64


;;
;; -- Calculate the length of the string
;;    ----------------------------------
kStrLen:
    cld
    push    rcx                                                 ;; save this register
    push    rdi                                                 ;; and this one

    mov     rcx,-1                                              ;; set max chars to scan (lots)
    xor     rax,rax                                             ;; al holds the char to find (NULL)

    repne   scasb                                               ;; find '\0', decrementing ecx as you go

    not     rcx                                                 ;; where did we start - take complement
    mov     rax,rcx                                             ;; move it to return
    dec     rax                                                 ;; and subtract one

    pop     rdi                                                 ;; restore the register
    pop     rcx                                                 ;; and this one
    ret
