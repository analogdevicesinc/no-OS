/***************************************************************************//**
 *   @file   Console.h
 *   @brief  Header file of Console Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/******************************************************************************/
/******************** Macros and Constants Definitions ************************/
/******************************************************************************/
#define UNKNOWN_CMD -1
#define DO_CMD       0
#define READ_CMD     1
#define WRITE_CMD    2

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*!< Initializes the serial console. */
char CONSOLE_Init(unsigned long baudRate);

/*!< Prints formatted data to console. */
void CONSOLE_Print(char* str, ...);

/*!< Reads one command from console. */
void CONSOLE_GetCommand(char* command);

/*!< Compares two commands and returns the type of the command. */
char CONSOLE_CheckCommands(char*       receivedCmd,
                           const char* expectedCmd,
                           double*     param,
                           char*       paramNo);

#endif /*__CONSOLE_H__*/
