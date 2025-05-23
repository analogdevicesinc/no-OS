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

#include "console.h"
#include "string.h"
#include "stdio.h"
#include "uart.h"

mxc_uart_regs_t* console_uart = MXC_UART0;

void consolePrepare(mxc_uart_regs_t* uart)
{
    // Save the UART to use
    console_uart = uart;

    MXC_UART_Init(uart, 115200, MXC_UART_APB_CLK);
}

void consoleStrOut(char* str)
{
    // Get the string's length.
    int len = strlen(str);

    // Send each byte in str to the console.
    for(int i = 0; i < len; i++)
    {
        // Line feed characters need to be prefixed with a carriage return.
        if(str[i] == '\n')
        {
            MXC_UART_WriteCharacter(console_uart, '\r');
        }
        MXC_UART_WriteCharacter(console_uart, str[i]);
    }
}

void consoleDecOut(int val)
{
    // Format the value as a decimal (base 10) string and output it.
    char str[11];
    sprintf(str, "%d", val);
    consoleStrOut(str);
}

int consoleReadVal(uint32_t* val)
{
    char str[11];
    int off = 0;

    do
    {
        // Retreive a byte and echo it.
        str[off] = MXC_UART_ReadCharacter(console_uart);
        MXC_UART_WriteCharacter(console_uart, str[off]);

        // Hande backspace characters specially.
        if(str[off] == 0x08)
        {
            // Make sure to not go back too far.
            if(off)
            {
                off--;
            }
        }
        else
        {
            off++;
        }
    } while((str[off - 1] != '\n') && (str[off -1] != '\r') && (off < 10));

    // Add a NULL character at the end of the string
    str[off] = 0;

    // Parse the value as a base 10 string.
    if(sscanf(str, "%d", val) != 1)
    {
        return 0;
    }

    return 1;
}

void consoleRead(uint8_t* buff, uint32_t len, int doEcho)
{
    // Read 'len' bytes.
    while(len)
    {
        // Get a byte.
        buff[0] = MXC_UART_ReadCharacter(console_uart);

        // Echo it if required.
        if(doEcho)
        {
            MXC_UART_WriteCharacter(console_uart, buff[0]);
        }

        buff++;
        len--;
    }
}

int consoleCharAvailable()
{
    // Check if the receive FIFO is empty.
    return (console_uart->status & MXC_F_UART_STATUS_RX_EM) == 0;
}

void consoleFlush()
{
    // Wait until the TX FIFO is empty.
    while((console_uart->status & MXC_F_UART_STATUS_TX_EM) == 0);
}