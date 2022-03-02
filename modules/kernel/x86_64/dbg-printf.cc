/****************************************************************************************************************//**
*   @file               dbg-printf.cc
*   @brief              Write a formatted string to the serial port COM1 (like printf)
*   @author             Adam Clark (hobbyos@eryjus.com)
*   @date               2022-Feb-26
*   @since              v0.0.0
*
*   @copyright          Copyright (c)  2022 -- Adam Clark\n
*                       Licensed under "THE BEER-WARE LICENSE"\n
*                       See \ref LICENSE.md for details.
*
*   Write a formatted string to the serial port.  This function works similar to `printf()` from the C runtime
*   library.
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
#include "internals.h"


//
// -- Several flags
//    -------------
enum {
    ZEROPAD = 1<<0,            /* pad with zero */
    SIGN    = 1<<1,            /* unsigned/signed long */
    PLUS    = 1<<2,            /* show plus */
    SPACE   = 1<<3,            /* space if plus */
    LEFT    = 1<<4,            /* left justified */
    SPECIAL = 1<<5,            /* 0x */
    LARGE   = 1<<6,            /* use 'ABCDEF' instead of 'abcdef' */
};



/****************************************************************************************************************//**
*   @typedef            va_list
*   @brief              A list of variable arguments
*///-----------------------------------------------------------------------------------------------------------------
typedef char *va_list;


/****************************************************************************************************************//**
*   @def                va_start()
*   @brief              Start a variable argument list
*///-----------------------------------------------------------------------------------------------------------------
#define va_start()      (curParm = 1)


/****************************************************************************************************************//**
*   @def                va_arg(t)
*   @brief              Get the next variable argument
*
*   @note               This macro has the side-effect of advancing the argument to the next
*///-----------------------------------------------------------------------------------------------------------------
#define va_arg(t)       (*(t *)&p[curParm ++])


/****************************************************************************************************************//**
*   @def                va_end()
*   @brief              Terminate the argument list
*///-----------------------------------------------------------------------------------------------------------------
#define va_end()



/****************************************************************************************************************//**
*   @var                digits
*   @brief              A list of lower-case hexidecimal numbers (extended to the end of the alphabet)
*///-----------------------------------------------------------------------------------------------------------------
static const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";


/****************************************************************************************************************//**
*   @var                upper_digits
*   @brief              A list of upper-case hexidecimal numbers (extended to the end of the alphabet)
*///-----------------------------------------------------------------------------------------------------------------
static const char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";


/********************************************************************************************************************
*   See `internals.h` for documentation
*///-----------------------------------------------------------------------------------------------------------------
int DbgPrintf(const char *fmt, ...)
{
    uint8_t pCnt;
    void *p[6];

    __asm volatile(" \
        mov %%rdi,%0\n \
        mov %%rsi,%1\n \
        mov %%rdx,%2\n \
        mov %%rcx,%3\n \
        mov %%r8,%4 \n \
        mov %%r9,%5 \n \
        mov %%al,%6 \n \
    " : "=m"(p[0]), "=m"(p[1]), "=m"(p[2]), "=m"(p[3]), "=m"(p[4]), "=m"(p[5]), "=m"(pCnt));


    if (pCnt > 6) {
        DbgPrintf("ERROR: Too many `kprintf()` parameters; use 6 or less!\n");
        return 0;
    }

    int printed = 0;
    int curParm = 1;
    const char *dig = digits;

    va_start();

    for (; *fmt; fmt ++) {
        bool leftJustify = false;
        bool showBase = false;
        bool big = false;
        int width = 0;          // minimum
        int precision = 999999; // maximum


        // -- check for special character sequences
        if (*fmt == '\\') {
            char ch = 0;

            fmt ++;

            switch (*fmt) {
            case '\\':
                SerialPutChar('\\');
                continue;

            case '"':
                SerialPutChar('"');
                continue;

            case 't':
                SerialPutChar('\t');
                continue;

            case 'n':
                SerialPutChar('\n');
                continue;

            case 'r':
                SerialPutChar('\r');
                continue;

            case 'x':
            case 'X':
                fmt ++;

                if (*fmt >= '0' && *fmt <= '9') ch = ch * 16 + (*fmt - '0');
                else if (*fmt >= 'a' && *fmt <= 'f') ch = ch * 16 + (*fmt - 'a' + 10);
                else if (*fmt >= 'A' && *fmt <= 'F') ch = ch * 16 + (*fmt - 'A' + 10);

                fmt ++;

                if (*fmt >= '0' && *fmt <= '9') ch = ch * 16 + (*fmt - '0');
                else if (*fmt >= 'a' && *fmt <= 'f') ch = ch * 16 + (*fmt - 'a' + 10);
                else if (*fmt >= 'A' && *fmt <= 'F') ch = ch * 16 + (*fmt - 'A' + 10);

                SerialPutChar(ch);
                continue;

            default:
                continue;
            }
        }

        // -- first, take care of anything other than the '%'
        if (*fmt != '%') {
            SerialPutChar(*fmt);
            continue;
        }

        // -- if we get here, we know we have a '%' format specifier; just increment past it
        fmt ++;

        // -- now, check for another '%'
        if (*fmt == '%') {
            SerialPutChar('%');
            continue;
        }

        // -- now, check if we will left justify
        if (*fmt == '-') {
            leftJustify = true;
            fmt ++;
        }

        // -- now we check for width digits; assume width for now
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt ++;
        }

        // -- check for a switch to precision
        if (*fmt == '.') {
            fmt ++;
            precision = 0;
        }

        // -- now we check for precision digits; assume width for now
        while (*fmt >= '0' && *fmt <= '9') {
            precision = precision * 10 + (*fmt - '0');
            fmt ++;
        }

        if (*fmt == '#') {
            fmt ++;
            showBase = true;
        }

        if (*fmt == 'l') {
            big = true;
            fmt ++;
        }

        // -- Finally we get the the format specifier
        switch (*fmt) {
        case 's':
            {
                const char *s = va_arg(const char *);
                if (!s) s = "(NULL)";

                int ex = width - kStrLen(s);

                if (!leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                while (*s && precision > 0) {
                    SerialPutChar(*s);
                    printed ++;
                    s ++;
                    precision --;
                }

                if (leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                continue;
            }

        case 'P':
            dig = upper_digits;
            // -- fall through

        case 'p':
            {
                Addr_t ptr = va_arg(Addr_t);

                if (showBase) {
                    SerialPutChar('0');
                    SerialPutChar(dig[33]);
                }

                for (int i = 15; i >= 0; i --) {
                    SerialPutChar(dig[(ptr >> (i << 2)) & 0xf]);
                    printed ++;
                }

                continue;
            }

        case 'd':
            if (big) {
                long val = va_arg(long);
                int p = 0;
                char n[40];

                if (val == 0) {
                    n[p++] = '0';
                } else {
                    while(val != 0) {
                        int dig = val % 10;
                        n[p++] = '0' + dig;
                        val /= 10;
                    }
                }

                int ex = width - p;

                if (!leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                p --;

                while (p >= 0) {
                    SerialPutChar(n[p]);
                    p --;
                    printed ++;
                }

                if (leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                continue;
            } else {
                int val = va_arg(int);
                int p = 0;
                char n[20];

                if (val == 0) {
                    n[p++] = '0';
                } else {
                    while(val != 0) {
                        int dig = val % 10;
                        n[p++] = '0' + dig;
                        val /= 10;
                    }
                }

                int ex = width - p;

                if (!leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                p --;

                while (p >= 0) {
                    SerialPutChar(n[p]);
                    p --;
                    printed ++;
                }

                if (leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                continue;
            }

        case 'u':
            if (big) {
                unsigned long val = va_arg(unsigned long);
                int p = 0;
                char n[40];

                if (val == 0) {
                    n[p++] = '0';
                } else {
                    while(val != 0) {
                        int dig = val % 10;
                        n[p++] = '0' + dig;
                        val /= 10;
                    }
                }

                int ex = width - p;

                if (!leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                p --;

                while (p >= 0) {
                    SerialPutChar(n[p]);
                    p --;
                    printed ++;
                }

                if (leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                continue;
            } else {
                unsigned int val = va_arg(unsigned int);
                int p = 0;
                char n[20];

                if (val == 0) {
                    n[p++] = '0';
                } else {
                    while(val != 0) {
                        int dig = val % 10;
                        n[p++] = '0' + dig;
                        val /= 10;
                    }
                }

                int ex = width - p;

                if (!leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                p --;

                while (p >= 0) {
                    SerialPutChar(n[p]);
                    p --;
                    printed ++;
                }

                if (leftJustify) {
                    while (ex > 0) {
                        SerialPutChar(' ');
                        ex --;
                    }
                }

                continue;
            }
        }
    }

    va_end();

    return printed;
}

#undef va_start
#undef va_arg
#undef va_end
#undef va_arg

