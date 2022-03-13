/****************************************************************************************************************//**
*   @file               internals.h
*   @brief              Some internal functions used within the kernel only
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Several items that are only available inside the kernel
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================


#ifndef __INTERNALS_H__
#define __INTERNALS_H__



#include "arch.h"



/****************************************************************************************************************//**
*   @def                ANSI_PASTE(x)
*   @brief              Macro to stringify the paramter
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_PASTE(x)           #x



/****************************************************************************************************************//**
*   @def                ANSI_ESC
*   @brief              The ANSI Escape Character sequence
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_ESC                "\x1b["



/****************************************************************************************************************//**
*   @def                ANSI_CLEAR
*   @brief              ANSI Sequence to clear the screen
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CLEAR              ANSI_ESC "2J"



/****************************************************************************************************************//**
*   @def                ANSI_SET_CURSOR(r,c)
*   @brief              ANSI Sequence to set the cursor position
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_SET_CURSOR(r,c)    ANSI_ESC ANSI_PASTE(r) ";" ANSI_PASTE(c) "H"



/****************************************************************************************************************//**
*   @def                ANSI_CURSOR_UP(x)
*   @brief              ANSI Sequence to move the cursor up some number of rows
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CURSOR_UP(x)       ANSI_ESC ANSI_PASTE(x) "A"



/****************************************************************************************************************//**
*   @def                ANSI_CURSOR_DOWN(x)
*   @brief              ANSI Sequence to move the cursor down some number of rows
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CURSOR_DOWN(x)     ANSI_ESC ANSI_PASTE(x) "B"



/****************************************************************************************************************//**
*   @def                ANSI_CURSOR_FORWARD(x)
*   @brief              ANSI Sequence to move the cursor forward (to the right) some number of columns
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CURSOR_FORWARD(x)  ANSI_ESC ANSI_PASTE(x) "C"



/****************************************************************************************************************//**
*   @def                ANSI_CURSOR_BACKWARD(x)
*   @brief              ANSI Sequence to move the cursor backward (to the left) some number of columns
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CURSOR_BACKWARD(x) ANSI_ESC ANSI_PASTE(x) "D"



/****************************************************************************************************************//**
*   @def                ANSI_CURSOR_SAVE
*   @brief              ANSI Sequence to save the cursor position
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CURSOR_SAVE        ANSI_ESC "s"



/****************************************************************************************************************//**
*   @def                ANSI_CURSOR_RESTORE
*   @brief              ANSI Sequence to restore the cursor position
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_CURSOR_RESTORE     ANSI_ESC "u"



/****************************************************************************************************************//**
*   @def                ANSI_ERASE_LINE
*   @brief              ANSI Sequence to erase to the end of the line
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_ERASE_LINE         ANSI_ESC "K"



/****************************************************************************************************************//**
*   @def                ANSI_ATTR_NORMAL
*   @brief              ANSI Sequence to set the normal attributes
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_ATTR_NORMAL        ANSI_ESC "0m"



/****************************************************************************************************************//**
*   @def                ANSI_ATTR_BOLD
*   @brief              ANSI Sequence to set the bold attribute
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_ATTR_BOLD          ANSI_ESC "1m"



/****************************************************************************************************************//**
*   @def                ANSI_ATTR_BLINK
*   @brief              ANSI Sequence to set the blink attribute
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_ATTR_BLINK         ANSI_ESC "5m"



/****************************************************************************************************************//**
*   @def                ANSI_ATTR_REVERSE
*   @brief              ANSI Sequence to set the reverse image attribute
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_ATTR_REVERSE       ANSI_ESC "7m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_BLACK
*   @brief              ANSI Sequence to set foreground color to black
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_BLACK           ANSI_ESC "30m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_RED
*   @brief              ANSI Sequence to set foreground color to red
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_RED             ANSI_ESC "31m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_GREEN
*   @brief              ANSI Sequence to set foreground color to green
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_GREEN           ANSI_ESC "32m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_YELLOW
*   @brief              ANSI Sequence to set foreground color to yellow
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_YELLOW          ANSI_ESC "33m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_BLUE
*   @brief              ANSI Sequence to set foreground color to blue
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_BLUE            ANSI_ESC "34m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_MAGENTA
*   @brief              ANSI Sequence to set foreground color to magenta
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_MAGENTA         ANSI_ESC "35m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_CYAN
*   @brief              ANSI Sequence to set foreground color to cyan
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_CYAN            ANSI_ESC "36m"



/****************************************************************************************************************//**
*   @def                ANSI_FG_WHITE
*   @brief              ANSI Sequence to set foreground color to white
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_FG_WHITE           ANSI_ESC "37m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_BLACK
*   @brief              ANSI Sequence to set background color to black
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_BLACK           ANSI_ESC "40m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_RED
*   @brief              ANSI Sequence to set background color to red
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_RED             ANSI_ESC "41m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_GREEN
*   @brief              ANSI Sequence to set background color to green
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_GREEN           ANSI_ESC "42m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_YELLOW
*   @brief              ANSI Sequence to set background color to yellow
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_YELLOW          ANSI_ESC "43m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_BLUE
*   @brief              ANSI Sequence to set background color to blue
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_BLUE            ANSI_ESC "44m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_MAGENTA
*   @brief              ANSI Sequence to set background color to magenta
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_MAGENTA         ANSI_ESC "45m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_CYAN
*   @brief              ANSI Sequence to set background color to cyan
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_CYAN            ANSI_ESC "46m"



/****************************************************************************************************************//**
*   @def                ANSI_BG_WHITE
*   @brief              ANSI Sequence to set background color to white
*///-----------------------------------------------------------------------------------------------------------------
#define     ANSI_BG_WHITE           ANSI_ESC "47m"



/****************************************************************************************************************//**
*   @fn                 int DbgPrintf(const char *fmt, ...)
*   @brief              `printf`-like function to output to the serial port
*
*   @param              fmt                 A formatted string
*
*   @returns            The numebr of characters actually printed to the serial port
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC __attribute__((format(printf,1,2)))
int DbgPrintf(const char *fmt, ...);



/****************************************************************************************************************//**
*   @fn                 void KernelPanic(const char *msg)
*   @brief              Panic Halt the Kernel
*
*   @param              msg                 The reason for halting the kernel
*
*   @note               This function does not return.
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC NORETURN
void KernelPanic(const char *msg);



/****************************************************************************************************************//**
*   @fn                 size_t kStrLen(const char *s)
*   @brief              Return the length of a string
*
*   @param              s                   The string for which to return the length
*
*   @returns            The string length less its terminating NULL.
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
size_t kStrLen(const char *s);



/****************************************************************************************************************//**
*   @fn                 void kMemMove(void *tgt, void *src, size_t cnt)
*   @brief              Move bytes of memory from one location to another
*
*   @param              tgt                 The destination of the move
*   @param              src                 The source memory location of the data to move
*   @param              cnt                 The number of bytes to move
*
*   This function will move memory from one location to another 1 byte at a time.  While inefficient, it is
*   guaranteed to work with minimal coding effort even if the memory regions overlap.
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void kMemMove(void *tgt, void *src, size_t cnt);



/****************************************************************************************************************//**
*   @fn                 void EarlyInit(void)
*   @brief              Complete the early initialization tasks
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void EarlyInit(void);



#endif


