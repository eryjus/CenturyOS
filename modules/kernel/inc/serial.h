/****************************************************************************************************************//**
*   @file               serial.h
*   @brief              Functions related to Serial Output
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2020-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
* ------------------------------------------------------------------------------------------------------------------
*
*   |     Date    | Tracker |  Version | Pgmr | Description
*   |:-----------:|:-------:|:--------:|:----:|:--------------------------------------------------------------------
*   | 2022-Feb-26 | Initial |  v0.0.0  | ADCL | Initial version
*
*///=================================================================================================================


#ifndef __SERIAL_H__
#define __SERIAL_H__


#include "arch.h"


/****************************************************************************************************************//**
*   @fn                 void SerialOpen(void)
*   @brief              Open the serial port
*
*   Open the serial port as "115200-n-8-1"
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void SerialOpen(void);



/****************************************************************************************************************//**
*   @fn                 void SerialPutChar(uint8_t ch)
*   @brief              Output a single character to the serial port
*
*   @param              ch              The character to output
*
*   Write a character to the serial port.
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void SerialPutChar(uint8_t ch);



/****************************************************************************************************************//**
*   @fn                 void SerialPutString(const char *s)
*   @brief              Output a string of characters to the serial port
*
*   @param              s               NULL-terminted pointer to the string to print
*
*   Write a string of characters to the serial port.
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void SerialPutString(const char *s);



/****************************************************************************************************************//**
*   @fn                 void SerialPutHex64(uint64_t h)
*   @brief              Output a 64-bit hexidecimal number to the serial port
*
*   @param              h               The number to print
*
*   Write a 64-bit hexidecimal number to the serial port, breaking it up into words.
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void SerialPutHex64(uint64_t h);



/****************************************************************************************************************//**
*   @fn                 void SerialPutHex32(uint32_t h)
*   @brief              Output a 32-bit hexidecimal number to the serial port
*
*   @param              h               The number to print
*
*   Write a 32-bit hexidecimal number to the serial port, breaking it up into words.
*///-----------------------------------------------------------------------------------------------------------------
extern "C" void SerialPutHex32(uint32_t h);


#endif

