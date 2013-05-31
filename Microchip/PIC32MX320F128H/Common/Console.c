/***************************************************************************//**
 *   @file   Console.c
 *   @brief  Implementation of Console Driver for PIC32MX320F128H.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Console.h"

/***************************************************************************//**
 * @brief Initializes the UART communication peripheral.
 *
 * @param baudRate - Baud rate value.
 *                   Example: 9600 - 9600 bps.
 *
 * @return status  - Result of the initialization procedure.
 *                   Example: -1 - if initialization was unsuccessful;
 *                             0 - if initialization was successful.
*******************************************************************************/
unsigned char UART_Init(unsigned long baudRate)
{
    unsigned long   pbFrequency = 80000000;
    unsigned short  brgValue    = 0;

    /* BaudRate = Fpb / (16 * (UxBRG + 1)) */
    brgValue = pbFrequency / (16 * baudRate) - 1;
    U1MODE   = 0;             // Clear the content of U1MODE register
    U1BRG    = brgValue;
    U1MODEbits.PDSEL1 = 0;
    U1MODEbits.PDSEL0 = 0;    // 8-bit data, no parity
    U1MODEbits.STSEL  = 0;    // 1 Stop bit
    U1STAbits.URXEN   = 1;    // Enable UART1 receiver
    U1STAbits.UTXEN   = 1;    // Enable UART1 transmitter
    U1MODEbits.ON     = 1;    // Enable UART1 peripheral
    
    return 0;
}

/***************************************************************************//**
 * @brief Writes one character to UART.
 *
 * @param data - Character to write.
 *
 * @return None.
*******************************************************************************/
void UART_Write(unsigned char data)
{
    while(U1STAbits.TRMT == 0);
    U1TXREG = data;
}

/***************************************************************************//**
 * @brief Reads one character from UART.
 *
 * @return receivedChar - Read character.
*******************************************************************************/
unsigned char UART_Read(void)
{
    unsigned char receivedChar = 0;

    while(U1STAbits.URXDA == 0);
    receivedChar = U1RXREG;

    return receivedChar;
}

/***************************************************************************//**
 * @brief Writes one string to UART.
 *
 * @param string - String to write.
 *
 * @return None.
*******************************************************************************/
void CONSOLE_WriteString(const char* string)
{
    while(*string)
    {
        UART_Write(*string++);
    }
}

/***************************************************************************//**
 * @brief Reads one command from UART.
 *
 * @param command - Read command.
 *
 * @return None.
*******************************************************************************/
void CONSOLE_GetCommand(unsigned char* command)
{
    unsigned char receivedChar = 0;
    unsigned char charNumber   = 0;
    
    while(receivedChar != 0x0D)
    {
        receivedChar = UART_Read();
        command[charNumber++] = receivedChar;
    }
}

/***************************************************************************//**
 * @brief Compares two commands and returns the type of the command.
 *
 * @param receivedCommand  - Received command.
 * @param expectedCommand  - Expected command.
 * @param commandParameter - Command parameter.
 *
 * @return commandType     - Type of the command.
 *                           Example: 0 - Commands don't match.
 *                                    1 - Write command.
 *                                    2 - Read command.
*******************************************************************************/
unsigned char CONSOLE_CheckCommands(unsigned char* receivedCommand,
                                    const char* expectedCommand,
                                    double* commandParameter)
{
    unsigned char commandType = 1;
    unsigned char charIndex   = 0;

    unsigned char parameterString[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char parameterIndex      = 0;

    while((expectedCommand[charIndex] != '?') &
          (expectedCommand[charIndex] != '=') &
          (commandType != 0))
    {
        if(expectedCommand[charIndex] != receivedCommand[charIndex])
        {
            commandType = 0;
        }
        charIndex++;
    }
    if(commandType != 0)
    {
        if(expectedCommand[charIndex] == '=')
        {
            if(receivedCommand[charIndex] == '=')
            {
                charIndex++;
                while(receivedCommand[charIndex] != 0x0D)
                {
                    parameterString[parameterIndex] = receivedCommand[charIndex];
                    charIndex++;
                    parameterIndex++;
                }
                *commandParameter = atof(parameterString);
            }
            else
            {
                commandType = 0;
            }
        }
        if(expectedCommand[charIndex] == '?')
        {
            if(receivedCommand[charIndex] == '?')
            {
                    commandType = 2;
            }
            else
            {
                    commandType = 0;
            }
        }
    }

    return commandType;
}
