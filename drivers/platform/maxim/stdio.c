/**
 * @file    stdio.c
 * @brief   Low level implementation of standard I/O functions
 */

/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2019-08-02 10:43:59 -0500 (Fri, 02 Aug 2019) $
 * $Revision: 45191 $
 *
 ******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Device and Board specific includes */
#include "mxc_config.h"
#include "mxc_sys.h"
#include "board.h"
//#include "uart.h"
#include "uart_x.h"
#include "uart_maxim.h"

/**
 * Address of the UART registers for the console output.
 * @note CONSOLE_UART must be defined in board.h.
 */
#define MXC_UARTn   MXC_UART_GET_UART(CONSOLE_UART)
/**
 * UART FIFO Address for the console UART.
 * @note CONSOLE_UART must be defined in board.h.
 */
#define UART_FIFO   MXC_UART_GET_FIFO(CONSOLE_UART)

/* Compiler Specific Includes */

/* GCC */
#if defined ( __GNUC__ )
#include <unistd.h>
#include <sys/stat.h>
#endif /* __GNUC__ */

/* Keil MDK Compiler */
#if defined ( __CC_ARM )
#include <time.h>
#include <rt_misc.h>
#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;
/**
 * Integer to store the last character read from the FILE using fgetc().
 * Only valid if fgetc() was the last function called on the stream.
 */
int g_lastChar = 0;
/**
 * Global variable set to TRUE if fgetc() was previously called, false otherwise.
 * This variable is necessary for implementing __backspace(FILE *f) in the MDK microlib.
 */
int g_readChar = 0;

#endif /* __CC_ARM */

/* Defines - Compiler Specific */
#if defined ( __ICCARM__ )
#define STDIN_FILENO    0   /**> Definition of stdin */
#define STDOUT_FILENO   1   /**> Definition of stdout */
#define STDERR_FILENO   2   /**> Definition of stderr */
#define EBADF          -1   /**> Error code for EBADf */
#endif /* __ICCARM__ */


/* The following libc stub functions are required for a proper link with printf().
 * These can be tailored for a complete stdio implementation.
 * GNUC requires all functions below. IAR & KEIL only use read and write.
 */
#if defined ( __GNUC__ )
int _open(const char *name, int flags, int mode)
{
    return -1;
}
int _close(int file)
{
    return -1;
}
int _isatty(int file)
{
    return -1;
}
int _lseek(int file, off_t offset, int whence)
{
    return -1;
}
int _fstat(int file, struct stat *st)
{
    return -1;
}
#endif /* __GNUC__ */

/* Handle IAR and ARM/Keil Compilers for _read/_write. Keil uses fputc and
   fgetc for stdio */
#if defined (__ICCARM__) || defined ( __GNUC__ )

#if defined ( __GNUC__ )                        // GNUC _read function prototype
int _read(int file, char *ptr, int len)
{
    int n;
#elif defined ( __ICCARM__ )                    // IAR Compiler _read function prototype
int __read(int file, unsigned char *ptr, size_t len)
{
    size_t n;
#endif /*  */

    int num = 0; // count of number received.
    
    switch (file)
    {
        case STDIN_FILENO:
            for (n = 0; n < len; n++) {
                *ptr = UART_ReadByte(MXC_UARTn); // read a byte.
                UART_WriteByte(MXC_UARTn,*ptr); // echo the byte.
                if (*ptr == '\r') { // check for end of line.
                    *ptr = '\n';
                    num++;
                    ptr++;

                    break;
                } else {
                    ptr++;
                    num++;
                }
            }

            break;
        default:
            errno = EBADF;
            return -1;
    }
    return num;
}

/* newlib/libc printf() will eventually call write() to get the data to the stdout */
#if defined ( __GNUC__ )
// GNUC _write function prototype
int _write(int file, char *ptr, int len)
{
    int n;
#elif defined ( __ICCARM__ )                // IAR Compiler _read function prototype
// IAR EW _write function prototype
int __write(int file, const unsigned char *ptr, size_t len)
{
    size_t n;
#endif /* __GNUC__ */


    switch (file) {
        case STDOUT_FILENO:
        case STDERR_FILENO:
            for (n = 0; n < len; n++) {
                if (*ptr == '\n') {
                    UART_WriteByte(MXC_UARTn,'\r');
                }
                UART_WriteByte(MXC_UARTn,*ptr++);
            }
//            UART_WriteAsync(MXC_UARTn, (uint8_t *)ptr, len);

            break;
        default:
            errno = EBADF;
            return -1;
    }

    return len;
}

#endif /* ( __ICCARM__ ) || ( __GNUC__ ) */

/* Handle Keil/ARM Compiler which uses fputc and fgetc for stdio */
#if defined ( __CC_ARM )
int fputc(int c, FILE *f)
{
    if(c != '\n') {
      UART_WriteByte(MXC_UARTn,c);
    } else {
      UART_WriteByte(MXC_UARTn,'\r');
      UART_WriteByte(MXC_UARTn,'\n');
    }

    return 0;
}

int __backspace(FILE *f)
{
    if (g_readChar)
        return g_lastChar;
    else
        return EOF;
}

int fgetc(FILE *f)
{
    g_lastChar = (int)UART_ReadByte(MXC_UARTn); /* Read the byte and save it to global for backspace */
    g_readChar = 1; /* set global to indicate g_lastChar is valid. */
    return g_lastChar;
}

int ferror(FILE *f)
{
    g_readChar = 0;
  return EOF;
}

void _ttywrch(int c)
{
    if(c != '\n') {
      UART_WriteByte(MXC_UARTn,c);
    } else {
      UART_WriteByte(MXC_UARTn,'\r');
      UART_WriteByte(MXC_UARTn,'\n');
    }
}

void _sys_exit(int return_code)
{
    while(1); /* endless loop for embedded micro */
}

#endif /* __CC_ARM  */
