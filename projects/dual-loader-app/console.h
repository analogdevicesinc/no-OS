/*******************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
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
 ******************************************************************************/

#include "uart.h"

/* Perform any initialization necessary for console input/output. */
void consolePrepare(mxc_uart_regs_t* uart);

/* Output the given string to the console. */
void consoleStrOut(char* str);

/* Output the given value (in base 10) to the console. */
void consoleDecOut(int val);

/* Read a value (base 10) from the console.  This function will read from the
   console until a new line character is received or 15 digits have been 
   entered.  This function returns 1 if successful, otherwise it returns 0. */
int consoleReadVal(uint32_t* val);

/* Read a value (base 16) from the console.  This function will read from the
   console until a new line character is received or 15 characters have been 
   entered.  This function returns 1 if successful, otherwise it returns 0. */
int consoleReadHex(uint32_t* val);

/* Reads 'len' number of bytes from the console and places them into 'buff'.  
   Set 'doEcho' to a non-zero value to echo back each byte as it is received.
   This function will block until all bytes are received. */
void consoleRead(uint8_t* buff, uint32_t len, int doEcho);

/* Determines if there are any characters that have been entered in the console
   but not yet consumed.  Returns non-zero if characters are available. */
int consoleCharAvailable(void);

/* Makes sure all characters that have been sent to the console have been
   output. */
void consoleFlush(void);
