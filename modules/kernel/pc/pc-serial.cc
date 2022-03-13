/****************************************************************************************************************//**
*   @file               pc-serial.cc
*   @brief              Serial functions for the pc
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Handle interacting with the serial port for the pc platform.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================



#include "arch.h"
#include "serial.h"



/****************************************************************************************************************//**
*   @def                COM1_BASE
*   @brief              The I/O Port for COM1
*///-----------------------------------------------------------------------------------------------------------------
#define COM1_BASE 0x3f8



/********************************************************************************************************************
*   Documented in `serial.h`
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void SerialOpen(void)
{
    OUTB(COM1_BASE + 1, 0x00);       // Disable all interrupts
    OUTB(COM1_BASE + 3, 0x80);       // Enable DLAB (set baud rate divisor)
    OUTB(COM1_BASE + 0, 0x01);       // Set divisor to 1 (lo byte) 115200 baud
    OUTB(COM1_BASE + 1, 0x00);       //                  (hi byte)
    OUTB(COM1_BASE + 3, 0x03);       // 8 bits, no parity, one stop bit
    OUTB(COM1_BASE + 2, 0xC7);       // Enable FIFO, clear them, with 14-byte threshold
    OUTB(COM1_BASE + 4, 0x0B);       // IRQs enabled, RTS/DSR set
}



/********************************************************************************************************************
*   Documented in `serial.h`
*///-----------------------------------------------------------------------------------------------------------------
KRN_FUNC
void SerialPutChar(uint8_t ch)
{
    if (ch == '\n') SerialPutChar('\r');

    while ((INB(COM1_BASE + 5) & 0x20) == 0) {}

    OUTB(COM1_BASE + 0, ch);
}


