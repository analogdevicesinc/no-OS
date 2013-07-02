/***************************************************************************//**
 *   @file   Console.c
 *   @brief  Implementation of Console Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdarg.h>
#include <stdlib.h>
#include "Communication.h"
#include "Console.h"

/***************************************************************************//**
 * @brief Converts an integer number to a string of ASCII characters string.
 *
 * @param number - Integer number.
 * @param base   - Numerical base used to represent the integer number as
 *                 string.
 *
 * @return Pointer to the string of ASCII characters.
*******************************************************************************/
char *intToStr(long number, char base)
{
    unsigned long posNumber  = 0;
    char          negSign    = 0;
    const char    digits[17] = "0123456789ABCDEF";
    static char   buffer[17] = "                ";
    char*         bufferPtr  = &buffer[16];

    if((number < 0) && (base == 10))
    {
        negSign = 1;
        posNumber = -1 * number;
    }
    else
    {
        posNumber = (unsigned long)number;
    }
    do
    {
        *bufferPtr-- = digits[posNumber % base];
        posNumber /= base;
    }
    while(posNumber != 0);
    if(negSign)
    {
        *bufferPtr-- = '-';
    }
    *bufferPtr++;

    return bufferPtr;
}

/***************************************************************************//**
 * @brief Prints formatted data to console.
 *
 * @param str - String to be printed.
 *
 * @return None.
*******************************************************************************/
void CONSOLE_Print(char* str, ...)
{
    char*         stringPtr;
    char          firstParam  = 0;
    char          secondParam = 0;
    unsigned long xMask       = 0;
    unsigned long dMask       = 0;
    char          chNumber    = 0;
    unsigned long multiplier  = 1;
    char*         strArg;
    long          longArg;
    double        doubleArg;
    va_list       argp;
    
    va_start(argp, str);
    for(stringPtr = str; *stringPtr != '\0'; stringPtr++)
    {
        if(*stringPtr!='%')
        {
            UART_WriteChar(*stringPtr);
            continue;
        }
        stringPtr++;
        firstParam = 0;
        while((*stringPtr >= 0x30) & (*stringPtr <= 0x39))
        {
            firstParam *= 10;
            firstParam += (*stringPtr - 0x30);
            stringPtr++;
        }
        if(*stringPtr == '.')
        {
            stringPtr++;
            secondParam = 0;
            while((*stringPtr >= 0x30) & (*stringPtr <= 0x39))
            {
                secondParam *= 10;
                secondParam += (*stringPtr - 0x30);
                stringPtr++;
            }
        }
        switch(*stringPtr)
        {
        case 'c':
            longArg = va_arg(argp, long);
            UART_WriteChar((char)longArg);
            break;
        case 's':
            strArg = va_arg(argp, char*);
            UART_WriteString(strArg);
            break;
        case 'd':
            longArg = va_arg(argp, long);
            UART_WriteString(intToStr(longArg, 10));
            break;
        case 'x':
            longArg = va_arg(argp, long);
            xMask = 268435456;
            chNumber = 8;
            while(xMask > longArg)
            {
                xMask /= 16;
                chNumber--;
            }
            while(chNumber < firstParam)
            {
                UART_WriteChar('0');
                chNumber++;
            }
            if(longArg != 0)
            {
                UART_WriteString(intToStr(longArg, 16));
            }
            break;
        case 'f':
            doubleArg = va_arg(argp, double);
            chNumber = secondParam;
            while(chNumber > 0)
            {
                multiplier *= 10;
                chNumber--;
            }
            doubleArg *= multiplier;
            if(doubleArg < 0)
            {
                doubleArg *= -1;
                UART_WriteChar('-');
            }
            longArg = (long)doubleArg;
            UART_WriteString(intToStr((longArg / multiplier), 10));
            UART_WriteChar('.');
            dMask = 1000000000;
            chNumber = 10;
            while(dMask > (long)(longArg % multiplier))
            {
                dMask /= 10;
                chNumber--;
            }
            while(chNumber < secondParam)
            {
                UART_WriteChar('0');
                chNumber++;
            }
            if((longArg % multiplier) != 0)
            {
                UART_WriteString(intToStr((longArg % multiplier), 10));
            }
            break;
        }
    }
    va_end(argp);
}

/***************************************************************************//**
 * @brief Initializes the serial console.
 *
 * @param baudRate - Baud rate value.
 *                   Example: 9600 - 9600 bps.
 *
 * @return status  - Result of the initialization procedure.
 *                   Example: -1 - if initialization was unsuccessful;
 *                             0 - if initialization was successful.
*******************************************************************************/
char CONSOLE_Init(unsigned long baudRate)
{
    return UART_Init(baudRate);
}

/***************************************************************************//**
 * @brief Reads one command from console.
 *
 * @param command - Read command.
 *
 * @return None.
*******************************************************************************/
void CONSOLE_GetCommand(char* command)
{
    char receivedChar = 0;
    char charNumber   = 0;
    
    while((receivedChar != '\n') && (receivedChar != '\r'))
    {
        UART_ReadChar(&receivedChar);
        command[charNumber++] = receivedChar;
    }
}

/***************************************************************************//**
 * @brief Compares two commands and returns the type of the command.
 *
 * @param receivedCmd - Received command.
 * @param expectedCmd - Expected command.
 * @param param       - Parameters' buffer.
 * @param paramNo     - Nomber of parameters.
 *
 * @return cmdType    - Type of the command.
 *                      Example: UNKNOWN_CMD - Commands don't match.
 *                               DO_CMD      - Do command (!).
 *                               READ_CMD    - Read command (?).
 *                               WRITE_CMD   - Write command (=).
*******************************************************************************/
char CONSOLE_CheckCommands(char*       receivedCmd,
                           const char* expectedCmd,
                           double*     param,
                           char*       paramNo)
{
    char cmdType         = 1;
    char charIndex       = 0;
    char paramString[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char paramIndex      = 0;
    char index           = 0;
    const char digits[17] = "0123456789ABCDEF";
    char digitIndex = 0;

    while((expectedCmd[charIndex] != '!') &&
          (expectedCmd[charIndex] != '?') &&
          (expectedCmd[charIndex] != '=') &&
          (cmdType != UNKNOWN_CMD))
    {
        if(expectedCmd[charIndex] != receivedCmd[charIndex])
        {
            cmdType = UNKNOWN_CMD;
        }
        charIndex++;
    }
    if(cmdType != UNKNOWN_CMD)
    {
        if(expectedCmd[charIndex] == '!')
        {
            if(receivedCmd[charIndex] == '!')
            {
                cmdType = DO_CMD;
            }
            else
            {
                cmdType = UNKNOWN_CMD;
            }
        }
        if(expectedCmd[charIndex] == '?')
        {
            if(receivedCmd[charIndex] == '?')
            {
                cmdType = READ_CMD;
            }
            else
            {
                cmdType = UNKNOWN_CMD;
            }
        }
        if(expectedCmd[charIndex] == '=')
        {
            if(receivedCmd[charIndex] == '=')
            {
                cmdType = WRITE_CMD;
            }
            else
            {
                cmdType = UNKNOWN_CMD;
            }
        }
        if((cmdType == WRITE_CMD) || (cmdType == READ_CMD))
        {
            charIndex++;
            while((receivedCmd[charIndex] != '\n') &&
                  (receivedCmd[charIndex] != '\r'))
            {
                if((receivedCmd[charIndex] == 0x20))
                {
                    *param = 0;
                    if((paramString[0] == '0') && (paramString[1] == 'x'))
                    {
                        for(index = 2; index < paramIndex; index++)
                        {
                            for(digitIndex = 0; digitIndex < 16; digitIndex++)
                            {
                                if(paramString[index] == digits[digitIndex])
                                {
                                    *param = *param * 16;
                                    *param = *param + digitIndex;
                                }
                            }
                        }
                    }
                    else
                    {
                        *param = atof((const char*)paramString);
                    }
                    param++;
                    *paramNo += 1;
                    for(paramIndex = 0; paramIndex < 10; paramIndex++)
                    {
                        paramString[paramIndex] = 0;
                    }
                    paramIndex = 0;
                    charIndex++;
                }
                else
                {
                    paramString[paramIndex] = receivedCmd[charIndex];
                    charIndex++;
                    paramIndex++;
                }
            }
            if(paramIndex)
            {
                *param = 0;
                if((paramString[0] == '0') && (paramString[1] == 'x'))
                {
                    for(index = 2; index < paramIndex; index++)
                    {
                        for(digitIndex = 0; digitIndex < 16; digitIndex++)
                        {
                            if(paramString[index] == digits[digitIndex])
                            {
                                *param *= 16;
                                *param += digitIndex;
                            }
                        }
                    }
                }
                else
                {
                    *param = atof((const char*)paramString);
                }
                *paramNo += 1;
            }
        }
    }

    return cmdType;
}
